#if !defined(__THREADPOOL_H__1EB64BC1_88A2_4E44_9EEA_15C47356230B__INCLUDE__)
#define __THREADPOOL_H__1EB64BC1_88A2_4E44_9EEA_15C47356230B__INCLUDE__

#pragma once

#include "config.h"
#include "atlmfc.h"
#include "utils.h"
#include "thread.h"


interface IThreadPoolTask
{
	virtual void process() = 0;
};//interface IThreadPoolTask

struct ThreadPool
{
protected:
	struct PoolWorkedThread : public CWorkedThreadImpl<PoolWorkedThread>
	{
		ThreadPool* m_pool;

		PoolWorkedThread(ThreadPool* _pool = NULL)
			:m_pool(_pool)
		{
			m_bAutoDelete = true;
			m_bAutoCleanup = false;
		}

		~PoolWorkedThread()
		{
			predestruct();
		}

		DWORD thread_main()
		{
			VERIFY_EXIT1(NOT_NULL(m_pool),0);

			for(;EQL(0,wait4event(m_pool->m_start_processing,get_stophandle(),INFINITE));)
			{
				m_pool->start_thread();
				IThreadPoolTask* task = NULL;
				for(task = m_pool->get_next_task()
					;NOT_NULL(task)
					;task = m_pool->get_next_task()
					)
				{
					task->process();
				}
				m_pool->end_thread();
				if(EQL(0,wait4event(get_stophandle(),m_pool->m_end_of_processing_thread,INFINITE))) break;
			}
			return 0;
		}
	};//struct PoolWorkedThread

	//friend struct ThreadPool::PoolWorkedThread;

protected:
	CEvent m_start_processing;
	CEvent m_end_of_processing;
	CEvent m_end_of_processing_thread;
	CCriticalSection m_critsect;
	LONG m_running_tasks;
	std::vector<IThreadPoolTask*> m_tasks;
	std::vector<PoolWorkedThread*> m_worked_threads;
	LONG m_task_idx;

public:
	ThreadPool(size_t _PoolSize = 2)
		:m_running_tasks(0)
		,m_task_idx(-1)
		,m_end_of_processing(FALSE,TRUE)
		,m_end_of_processing_thread(FALSE,TRUE)
		,m_start_processing(FALSE,TRUE)
	{
		size_t i = 0;
		m_worked_threads.resize(_PoolSize,NULL);
		for(i=0;i<_PoolSize;i++)
		{
			m_worked_threads[i] = trace_alloc(new PoolWorkedThread());
			m_worked_threads[i]->m_pool = this;
			m_worked_threads[i]->start();	
			Sleep(0);
		}
	}

	virtual ~ThreadPool()
	{
		size_t i = 0;	
		for(i=0;i<m_worked_threads.size();i++)
		{
			VERIFY_DO(NOT_NULL(m_worked_threads[i]),continue);
			m_worked_threads[i]->abort();
		}
		m_worked_threads.clear();
	}

	template<typename _Iterator>
	void start_processing(_Iterator _it,_Iterator _ite)
	{
		VERIFY_EXIT(!is_signaled(m_start_processing));
		VERIFY_EXIT(EQL(m_running_tasks,0));
		clear();
		populate(_it,_ite);
		m_end_of_processing_thread.ResetEvent();
		m_start_processing.SetEvent();	
	}

	void wait_end_of_processing(DWORD _millisec = INFINITE)
	{
		VERIFY_EXIT(is_signaled(m_start_processing));
		if(EQL(0,wait4event(m_end_of_processing,_millisec)))
		{
			clear();
			reset();
			return;
		}
	}

	bool is_processing()
	{
		return is_signaled(m_start_processing);
	}

protected:
	void clear()
	{
		CAutoLock __al(m_critsect);
		m_task_idx = -1;
		m_tasks.clear();
	}

	void reset()
	{
		CAutoLock __al(m_critsect);
		m_start_processing.ResetEvent();
		m_end_of_processing.ResetEvent();
		m_end_of_processing_thread.SetEvent();
	}

	template<typename _Iterator>
	void populate(_Iterator _it,_Iterator _ite)
	{
		CAutoLock __al(m_critsect);
		size_t sz = std::distance(_it,_ite),i=0;
		m_tasks.resize(sz,NULL);
		for(i=0;_it!=_ite;++_it,i++)
		{
			m_tasks[i] = static_cast<IThreadPoolTask*>(&*_it);
		}
	}

	IThreadPoolTask* get_next_task()
	{
		CAutoLock __al(m_critsect);
		LONG task_idx = InterlockedIncrement(&m_task_idx);
		if(task_idx>=(LONG)m_tasks.size()) 
		{
			InterlockedDecrement(&m_task_idx);
			return NULL;
		}
		return m_tasks[task_idx];
	}

	void start_thread()
	{
		CAutoLock __al(m_critsect);
		InterlockedIncrement(&m_running_tasks);
	}

	void end_thread()
	{
		CAutoLock __al(m_critsect);
		if(InterlockedDecrement(&m_running_tasks)==0)
		{
			m_end_of_processing.SetEvent();
		}
	}
};//struct ThreadPool





#endif