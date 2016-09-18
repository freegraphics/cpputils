#pragma once

#include <utils/utils.h>
#include <utils/fileutils.h>
#include <utils/queue.h>
#include "DLNet.h"
#include <list>
#include <vector>

namespace RL
{
	// presentation class that can do Reinforcment Learning 
	template<typename _State>
	interface ILearn
	{
		// start new episode on learning
		virtual void start_new_episode() = 0;

		// get best next state
		virtual void get_best_state(_State& _best) = 0;

		// get next rand state
		virtual void get_rand_state(_State& _best) = 0;

		// next state
		virtual void find_next_train_state(_State& _best) = 0;

		// remember state
		virtual void fix_state(const _State& _state) = 0;

		// set stimulation for remebered state
		virtual void set_stimulation(double _stimulation) = 0;

		// id methods
		virtual void fix_state_with_id(size_t _id,const _State& _state) = 0;
		virtual void set_stimulation_with_id(size_t _id,double _stimulation) = 0;
		virtual void clear_states_before_id(size_t _id) = 0;
	};//interface ILearn

	// need for presentation of class that can set/get stimulation
	template<typename _State>
	interface IStateStimulation
	{
		// get stimulation 
		virtual double get_value(const _State& _state) = 0;

		// set stimulation
		virtual void set_value(const _State& _state,double _value) = 0;

		// return true if stimulation is zero 
		virtual bool is_zero_value(double _value) const = 0;
	};//interface IStimulation

	struct ValueBounds
	{
	protected:
		double value_min;
		double value_max;
	public:

		ValueBounds(double _value_min = 0
			,double _value_max = 1
			)
			:value_min(_value_min)
			,value_max(_value_max)
		{
		}

		double normalize(double _natural_value) const
		{
			double d = (value_max-value_min);
			if(eql(d,0)) 
				return (_natural_value - value_min);
			else 
				return (_natural_value - value_min)/d;
		}

		double naturalize(double _normalized_value) const
		{
			return _normalized_value*(value_max-value_min)+value_min;
		}
	};//struct ValueMinMax

	namespace Private
	{
		struct Stimulation
		{
		protected:
			double value;
		public:

			Stimulation()
				:value(0)
			{}

			void set(double _val,const ValueBounds& _bounds = ValueBounds())
			{
				value = clamp<double>(_bounds.normalize(_val),0,1);
			}

			double get(const ValueBounds& _bounds = ValueBounds())
			{
				return _bounds.naturalize(value);
			}

			void to_vec(std::vector<double>& _vec,long& _idx) const
			{
				VERIFY_VEC_SIZE();
				_vec[_idx++] = clamp(value-0.5,-0.5,0.5);
			}

			void to_vec(std::vector<double>& _vec) const 
			{
				long idx = 0;
				to_vec(_vec,idx);
			}

			void from_vec(const std::vector<double>& _vec,long& _idx)
			{
				VERIFY_VEC_SIZE();
				value = _vec[_idx++]+0.5;//clamp<double>(_vec[_idx++]+0.5,0,1);
			}

			void from_vec(const std::vector<double>& _vec)
			{
				long idx = 0;
				from_vec(_vec,idx);
			}

			static size_t size()
			{
				return 1;
			}
		};
	};//namespace Private

	template<typename _State,class _Consts>
	struct SimpleSearchBest
	{
		void get(_State& _best,IStateStimulation<_State>* _stimulation) const
		{
			VERIFY_EXIT(NOT_NULL(_stimulation));
			size_t i = 0;
			_State best_state = _best;
			best_state.rand();
			double max_stimulation = _stimulation->get_value(best_state);
			for(i=0;i<_Consts::get().RL_SimpleSearchBest_search_depth;i++)
			{
				_State state = _best;
				state.rand();
				double stimulation = _stimulation->get_value(state);
				if(stimulation>max_stimulation)
				{
					best_state = state;
					max_stimulation = stimulation;
				}
			}
			_best = best_state;
		}
	};//template<> struct SimpleSearchBest

	namespace GA
	{
		namespace Private
		{
			template<typename _State>
			struct Item
			{
				_State state;
				double stimulation;
				bool new_item_flag;

				Item()
					:stimulation(0)
					,new_item_flag(true)
				{
				}

				bool operator == (const Item<_State>& _state) const
				{
					return stimulation == _state.stimulation;
				}

				bool operator < (const Item<_State>& _state) const
				{
					return stimulation < _state.stimulation;
				}
			};

			template<typename _State>
			struct ItemPtr
			{
				Item<_State>* ptr;

				ItemPtr()
					:ptr(NULL)
				{
				}

				bool operator==(const ItemPtr<_State>& _ga_item_ptr) const 
				{
					if(ptr!=_ga_item_ptr.ptr)return false; 
					if(IS_NULL(ptr)) return true;
					return *ptr==*(_ga_item_ptr.ptr);
				}

				bool operator<(const ItemPtr<_State>& _ga_item_ptr) const 
				{
					if(IS_NULL(ptr) && NOT_NULL(_ga_item_ptr.ptr)) return true;
					if(IS_NULL(ptr) || IS_NULL(_ga_item_ptr.ptr)) return false;
					return *ptr<*(_ga_item_ptr.ptr);
				}

				bool operator > (const ItemPtr<_State>& _ga_item_ptr) const
				{
					return _ga_item_ptr < *this;
				}
			};

			template<typename _State>
			struct Items
			{
				typedef std::list<Item<_State> > ItemsLst;
				typedef std::vector<ItemPtr<_State> > Sorted;

				ItemsLst items;
				Sorted sorted;
				double stimulation_sum;
				_State& from;

				Items(_State& _from)
					:from(_from)
					,stimulation_sum(0)
				{
				}

				void rand(size_t _idx)
				{
					ItemPtr<_State> item = get_item(_idx);
					VERIFY_EXIT(NOT_NULL(item.ptr));
					item.ptr->state.rand();
					item.ptr->new_item_flag = true;
				}

				void create(size_t _idx)
				{
					ItemPtr<_State> item = get_item(_idx);
					VERIFY_EXIT(NOT_NULL(item.ptr));
				}

				void process(IStateStimulation<_State>* _stimulation)
				{
					VERIFY_EXIT(NOT_NULL(_stimulation));
					ItemsLst::iterator
						it = items.begin()
						,ite = items.end()
						;
					stimulation_sum = 0;
					for(;it!=ite;++it)
					{
						if(it->new_item_flag)
						{
							it->stimulation = _stimulation->get_value(it->state);
						}
						if(it->stimulation>0)
							stimulation_sum += it->stimulation;
					}
				}

				void sort()
				{
					VERIFY_EXIT(sorted.size()==items.size());
					std::sort(sorted.begin(),sorted.end(),std::greater<ItemPtr<_State> >());
				}

				void crossover(Items<_State>& _crossover_items,size_t _crossover_idx)
				{
					ItemPtr<_State> item = _crossover_items.get_item(_crossover_idx);
					VERIFY_EXIT(NOT_NULL(item.ptr));
					item.ptr->state.crossover(get_rand_state(),get_rand_state());
				}

				void copy(Items<_State>& _crossover_items,size_t _crossover_idx,size_t _idx)
				{
					ItemPtr<_State> item_crossover = _crossover_items.get_item(_crossover_idx);
					VERIFY_EXIT(NOT_NULL(item_crossover.ptr));
					ItemPtr<_State> item = get_item(_idx);
					VERIFY_EXIT(NOT_NULL(item.ptr));
					item.ptr->state = item_crossover.ptr->state;
					item.ptr->new_item_flag = true;
				}

				void mutate(size_t _idx)
				{
					ItemPtr<_State> item = get_item(_idx);
					VERIFY_EXIT(NOT_NULL(item.ptr));
					item.ptr->state.mutate();
					item.ptr->new_item_flag = true;
				}

				const _State& get_best() const
				{
					VERIFY_EXIT1(!sorted.empty() && NOT_NULL(sorted[0].ptr),Fish<_State>::get());
					return sorted[0].ptr->state;
				}

			protected:
				ItemPtr<_State> get_item(size_t _idx)
				{
					VERIFY_EXIT1(_idx<=items.size() && items.size()==sorted.size(),ItemPtr<_State>());
					if(_idx==items.size())
					{
						items.push_back(GA::Private::Item<_State>());
						sorted.push_back(GA::Private::ItemPtr<_State>());
						items.back().state = from;
						sorted.back().ptr = &items.back();
						return sorted.back();
					}
					return sorted[_idx];
				}

				const _State& get_rand_state()
				{
					VERIFY_EXIT1(sorted.size()==items.size(),Fish<_State>::get());
					double v = ::rand(stimulation_sum);
					Sorted::iterator 
						it = sorted.begin()
						,ite = sorted.end()
						;
					double sum = 0;
					for(;it!=ite;++it)
					{
						if(v<=sum) return it->ptr->state;
						if(it->ptr->stimulation>0)
							sum += it->ptr->stimulation;
					}
					--ite;
					return ite->ptr->state;
				}
			};
		};//namespace Private

		template<typename _State,class _Consts>
		struct SearchBest
		{
			void get(_State& _best,IStateStimulation<_State>* _stimulation) const
			{
				VERIFY_EXIT(NOT_NULL(_stimulation));
				size_t i=0;
				Private::Items<_State> items(_best);
				Private::Items<_State> crossover_items(_best);
				for(i=0;i<_Consts::get().RL_GASearchBest_Items;i++)
					items.rand(i);
				for(i=0;i<_Consts::get().RL_GASearchBest_CrossoverItems_per_cycle;i++)
					crossover_items.create(i);

				for(i=0;;i++)
				{
					items.process(_stimulation);
					items.sort();

					if(i>=_Consts::get().RL_GASearchBest_Cycles) break;

					long j = 0,k = 0;
					for(k=0,j=(long)_Consts::get().RL_GASearchBest_Items-1
						;j>=0 && k<(long)_Consts::get().RL_GASearchBest_CrossoverItems_per_cycle
						;k++,j--
						)
					{
						items.crossover(crossover_items,(size_t)k);
					}
					for(k=0,j=(long)_Consts::get().RL_GASearchBest_Items-1
						;j>=0 && k<(long)_Consts::get().RL_GASearchBest_CrossoverItems_per_cycle
						;k++,j--
						)
					{
						items.copy(crossover_items,(size_t)k,(size_t)j);
					}
					for(k=0
						;j>=0 && k<(long)_Consts::get().RL_GASearchBest_MutateItems_per_cycle
						;k++,j--
						)
					{
						items.mutate((size_t)j);
					}
					for(k=0
						;j>=0 && k<(long)_Consts::get().RL_GASearchBest_RandItems_per_cycle
						;k++,j--
						)
					{
						items.rand((size_t)j);
					}
				}
				_best = items.get_best();
			}
		};
	};//namespace GA

	template<class _State,class _Consts>
	struct MonteCarlo
	{
	protected:
		Queue<_State> states;
		IStateStimulation<_State>* istimulation;
	public:
		enum {optimization_tip_need_best = false};

		MonteCarlo(IStateStimulation<_State>* _istimulation)
			:istimulation(_istimulation)
		{
			states.resize(_Consts::get().RL_LearnNet_queue_min_size);
		}

		void start_new_episode()
		{
			states.clear();
		}

		void fix_current_state(const _State& _state)
		{
			states.push(_state);
		}

		void fix_current_state(size_t _id,const _State& _state)
		{
		}

		void fix_best_state(const _State& _best_state)
		{
			_best_state;
		}

		void set_stimulation(double _stimulation_value)
		{
			VERIFY_EXIT(NOT_NULL(istimulation));
			size_t i=0;
			for(i=0;i<states.size();i++)
			{
				istimulation->set_value(states.get(i),_stimulation_value);
			}
		}

		void set_stimulation(size_t _id,double _stimulation)
		{
		}

		void clear_states_before_id(size_t _id)
		{
		}
	};//template<> struct MonteCarlo

	template<class _State,class _Consts>
	struct TD1
	{
	protected:
		_State current_state;
		_State best_state;
		double stimulation;
		IStateStimulation<_State>* istimulation;

	public:
		enum {optimization_tip_need_best = true};

		TD1(IStateStimulation<_State>* _istimulation)
			:istimulation(_istimulation)
			,stimulation(0)
		{
		}

		void start_new_episode()
		{
			stimulation = 0;
		}

		void fix_current_state(const _State& _state)
		{
			current_state = _state;
		}

		void fix_current_state(size_t _id,const _State& _state)
		{
		}

		void fix_best_state(const _State& _best_state)
		{
			best_state = _best_state;

			VERIFY_EXIT(NOT_NULL(istimulation));
			double Vtcur = istimulation->get_value(best_state);
			double Vtprev = stimulation + _Consts::get().RL_TD1_ro*Vtcur;
			istimulation->set_value(current_state,Vtprev);
		}

		void set_stimulation(double _stimulation_value)
		{
			stimulation = _stimulation_value;
		}

		void set_stimulation(size_t _id,double _stimulation)
		{
		}

		void clear_states_before_id(size_t _id)
		{
		}
	};

	template<class _State>
	struct StatesPair
	{
		size_t id;
		_State from;
		_State to;
		double stimulation;

		StatesPair(size_t _id = -1,const _State& _from = _State(),const _State& _to = _State(),double _stimulation = 0)
			:id(_id)
			,from(_from)
			,to(_to)
			,stimulation(_stimulation)
		{
		}
	};

	template<class _State,class _Consts>
	struct TD1WithHistory
	{
	protected:
		_State current_state;
		_State best_state;
		double stimulation;
		IStateStimulation<_State>* istimulation;
		Queue<StatesPair<_State> > queue;
		//typedef std::pair<_State,_State> StatePair;
		//typedef std::map<size_t,StatePair> HistoryMap;
		//HistoryMap history;
		size_t last_id;
		_State from_state;
		bool need_to_set_best_state;

	public:
		enum {optimization_tip_need_best = true};

		TD1WithHistory(IStateStimulation<_State>* _istimulation)
			:istimulation(_istimulation)
			,stimulation(0)
			,last_id(0)
			,need_to_set_best_state(false)
		{
			queue.resize(8);
		}

		void start_new_episode()
		{
			stimulation = 0;
		}

		void fix_current_state(const _State& _state)
		{
			current_state = _state;
		}

		void fix_current_state(size_t _id,const _State& _state)
		{
			last_id = _id;
			from_state = _state;
			need_to_set_best_state = true;
		}

		void fix_best_state(const _State& _best_state)
		{
			best_state = _best_state;
			if(need_to_set_best_state)
			{
				if(queue.size()<_Consts::get().RL_LearnNet_max_queue_size)
					queue.push(StatesPair<_State>(last_id,from_state,_best_state,stimulation));
				else
				{
					size_t idx = ::rand()%queue.size();
					queue.set(idx,StatesPair<_State>(last_id,from_state,_best_state,stimulation));
				}
				need_to_set_best_state = false;
			}

			if(queue.size()>=_Consts::get().RL_LearnNet_min_queue_size)
			{
				VERIFY_EXIT(NOT_NULL(istimulation));
				size_t i = 0;
				for(i=0;i<_Consts::get().RL_LearnNet_batch_size;i++)
				{
					size_t idx = ::rand()%queue.size();
					const StatesPair<_State>& sp = queue.get(idx);
					double Vtcur = istimulation->get_value(sp.to);
					double Vtprev = stimulation + _Consts::get().RL_TD1_ro*Vtcur;
					istimulation->set_value(sp.from,Vtprev);
				}
			}
		}

		void set_stimulation(double _stimulation_value)
		{
			stimulation = _stimulation_value;
		}

		void set_stimulation(size_t _id,double _stimulation)
		{
			size_t i = 0;
			for(i=0;i<queue.size();i++)
			{
				if(queue.get(i).id==_id)
				{
					queue.get(i).stimulation = _stimulation;
					break;
				}
			}
		}

		void clear_states_before_id(size_t _id)
		{
		}
	};

	template<typename _State>
	struct StateTraits
	{
		typedef RL::Private::Stimulation Rewards;

		static double get_value(const _State& _state,Net& _net,const ValueBounds& _stimulation_bounds)
		{
			_state.to_vec(_net.get_input().w);
			_net.forward();
			Rewards stim;
			stim.from_vec(_net.get_result().w);
			return stim.get(_stimulation_bounds);
		}

		template<typename _Consts>
		static void set_value(const _State& _state,double _value,Net& _net,SGDTrainer& _trainer,double& _loss,const ValueBounds& _stimulation_bounds,const _Consts& _consts)
		{
			Rewards stim;
			stim.set(_value,_stimulation_bounds);
			_state.to_vec(_net.get_input().w);
			stim.to_vec(_net.get_target().w);

			DWORD fwd_time = 0,bwd_time = 0;
			double l2_decay_loss = 0,l1_decay_loss = 0,cost_loss = 0,loss1 = 0;
			_trainer.train(fwd_time,bwd_time,l2_decay_loss,l1_decay_loss,cost_loss,loss1);
			_loss += (cost_loss - _loss)*_Consts::get().RL_LearnNet_loss_avg_coef;
		}
	};

	template<
		class _State
		,class _SearchBest = SimpleSearchBest<_State,Consts>
		,class _RLAlg = MonteCarlo<_State,Consts>
		,class _Consts = Consts
	>
	struct LearnNet 
		:ILearn<_State>
		,IStateStimulation<_State>
	{
	protected:
		_SearchBest search_best_functor;
		_RLAlg rl_alg_functor;
		Net net;
		SGDTrainer trainer;
		Net get_net;
		double loss;
		size_t train_idx;

	public:
		ValueBounds stimulation_bounds;

	public:
		LearnNet()
			:trainer(net)
			,loss(0)
#pragma warning(push)
#pragma warning(disable: 4355)
			,rl_alg_functor(this)
#pragma warning(pop)
			,train_idx(0)
		{
			init_net();
		}

		~LearnNet()
		{
			free();
		}

		void free()
		{
			net.free();
		}

		double get_loss() const {return loss;}

		//interface ILearn
		//{
		virtual void start_new_episode()
		{
			rl_alg_functor.start_new_episode();
		}

		virtual void get_best_state(_State& _best)
		{
			search_best_functor.get(_best,this);
		}

		virtual void get_rand_state(_State& _best)
		{
			_best.rand();
		}

		virtual void find_next_train_state(_State& _state)
		{
			if(rl_alg_functor.optimization_tip_need_best)
			{
				get_best_state(_state);
				rl_alg_functor.fix_best_state(_state);
			}
			if(::rand(1.0)<_Consts::get().RL_LearnNet_search_coef)
				get_rand_state(_state);
			else if(!rl_alg_functor.optimization_tip_need_best)
				get_best_state(_state);
		}

		virtual void fix_state(const _State& _state)
		{
			rl_alg_functor.fix_current_state(_state);
		}

		virtual void set_stimulation(double _stimulation)
		{
			rl_alg_functor.set_stimulation(_stimulation);
		}

		virtual void fix_state_with_id(size_t _id,const _State& _state)
		{
			rl_alg_functor.fix_current_state(_id,_state);
		}

		virtual void set_stimulation_with_id(size_t _id,double _stimulation)
		{
			rl_alg_functor.set_stimulation(_id,_stimulation);
		}

		virtual void clear_states_before_id(size_t _id)
		{
			rl_alg_functor.clear_states_before_id(_id);
		}

		//};//interface ILearn

		//interface IStateStimulation
		//{
		virtual double get_value(const _State& _state)
		{
			return StateTraits<_State>::get_value(_state,get_net,stimulation_bounds);
		}

		virtual void set_value(const _State& _state,double _value)
		{
			StateTraits<_State>::set_value(_state,_value,net,trainer,loss,stimulation_bounds,_Consts::get());
			train_idx++;
			if(train_idx>_Consts::get().RL_LearnNet_train_cycles_to_switch_net)
			{
				train_idx = 0;
				get_net.copy_from(net);
			}
		}

		virtual bool is_zero_value(double _value) const
		{
			return eql(stimulation_bounds.normalize(_value),0);
		}
		//};//interface IStateStimulation

		template<typename _Stream>
		void save(_Stream& _out) const
		{
			net.save(_out);
		}

		template<typename _Stream>
		void load(_Stream& _in)
		{
			net.load(_in);
			get_net.copy_from(net);
		}

		void save(LPCTSTR _file_name) const
		{
			std::tofstream out;
			out.open(_file_name);
			save(out);
		}

		void load(LPCTSTR _file_name)
		{
			if(!FileUtils::IsFileExist(_file_name)) return;
			std::tifstream inp;
			inp.open(_file_name);
			load(inp);
		}

		void save(const CString& _file_name) const
		{
			save((LPCTSTR)_file_name);
		}

		void load(const CString& _file_name)
		{
			load((LPCTSTR)_file_name);
		}

	protected:
		void init_net()
		{
			TrainerOptions to;
			to.learning_rate = _Consts::get().RL_LearnNet_learning_rate;
			to.momentum = _Consts::get().RL_LearnNet_momentum;
			to.l2_decay = _Consts::get().RL_LearnNet_l2_decay;
			to.batch_size = _Consts::get().RL_LearnNet_batch_size;
			trainer.set_options(to);

			LayerOptsLst layer_defs;
			{
				LayerOpts opt;
				InputLayer::init(opt);
				opt.type = layertype_input;
				opt.out_sx = 1;
				opt.out_sy = 1;
				opt.out_depth = _State::size();
				layer_defs.push_back(opt);
			}
			{
				LayerOpts opt;
				FullyConnLayer::init(opt);
				//ReluLayer::init(opt);
				init_layer_options(_Consts::get().RL_LearnNet_net_hidden_layers_type,opt);
				opt.type = layertype_fc;
				opt.activation = _Consts::get().RL_LearnNet_net_hidden_layers_type;//layertype_relu;
				opt.num_neurons = _Consts::get().RL_LearnNet_net_hidden_layers_size;
				size_t i=0;
				for(i=0;i<_Consts::get().RL_LearnNet_net_hidden_layers_count;i++)
				{
					layer_defs.push_back(opt);
				}
			}
			{
				LayerOpts opt;
				RegressionLayer::init(opt);
				opt.type = layertype_regression;
				opt.num_neurons = StateTraits<_State>::Rewards::size();
				layer_defs.push_back(opt);
			}
			net.makeLayers(layer_defs);
			get_net.makeLayers(layer_defs);
		}
	};//template<> struct LearnNet

};//namespace RL

/* 
struct Consts // RL consts template
{
public:
	size_t RL_SimpleSearchBest_search_depth;
	size_t RL_GASearchBest_Items;
	size_t RL_GASearchBest_Cycles;
	size_t RL_GASearchBest_CrossoverItems_per_cycle;
	size_t RL_GASearchBest_MutateItems_per_cycle;
	size_t RL_GASearchBest_RandItems_per_cycle;
	size_t RL_LearnNet_queue_min_size;
	double RL_TD1_ro;
	double RL_LearnNet_loss_avg_coef;
	double RL_LearnNet_search_coef;
	double RL_LearnNet_learning_rate;
	double RL_LearnNet_momentum;
	double RL_LearnNet_l2_decay;
	size_t RL_LearnNet_batch_size;
	LayerTypeEn RL_LearnNet_net_hidden_layers_type;
	size_t RL_LearnNet_net_hidden_layers_size;
	size_t RL_LearnNet_net_hidden_layers_count;

protected:
	Consts()
	{
		RL_SimpleSearchBest_search_depth = 100;
		RL_GASearchBest_Items = 30;
		RL_GASearchBest_Cycles = 100;
		RL_GASearchBest_CrossoverItems_per_cycle = 10;
		RL_GASearchBest_MutateItems_per_cycle = 3;
		RL_GASearchBest_RandItems_per_cycle = 3;
		RL_LearnNet_queue_min_size = 16;
		RL_TD1_ro = 1.0;
		RL_LearnNet_loss_avg_coef = 1e-3;
		RL_LearnNet_search_coef = 0.3;
		RL_LearnNet_learning_rate = 0.1;
		RL_LearnNet_momentum = 0.9;
		RL_LearnNet_l2_decay = 0;
		RL_LearnNet_batch_size = 1;
		RL_LearnNet_net_hidden_layers_type = layertype_relu;
		RL_LearnNet_net_hidden_layers_size = 20;
		RL_LearnNet_net_hidden_layers_count = 6;
	}
	static Consts& get_instance()
	{
		static Consts _;
		return _;
	}
public:
	static const Consts& get()
	{
		return get_instance();
	}

	template<typename _Stream>
	void print(_Stream& _out) const 
	{
		OUT_VALUE(_out,RL_SimpleSearchBest_search_depth);
		OUT_VALUE(_out,RL_GASearchBest_Items);
		OUT_VALUE(_out,RL_GASearchBest_Cycles);
		OUT_VALUE(_out,RL_GASearchBest_CrossoverItems_per_cycle);
		OUT_VALUE(_out,RL_GASearchBest_MutateItems_per_cycle);
		OUT_VALUE(_out,RL_GASearchBest_RandItems_per_cycle);
		OUT_VALUE(_out,RL_LearnNet_queue_min_size);
		OUT_VALUE(_out,RL_TD1_ro);
		OUT_VALUE(_out,RL_LearnNet_loss_avg_coef);
		OUT_VALUE(_out,RL_LearnNet_search_coef);
		OUT_VALUE(_out,RL_LearnNet_learning_rate);
		OUT_VALUE(_out,RL_LearnNet_momentum);
		OUT_VALUE(_out,RL_LearnNet_l2_decay);
		OUT_VALUE(_out,RL_LearnNet_batch_size);
		OUT_VALUE_CONV(_out,RL_LearnNet_net_hidden_layers_type);
		OUT_VALUE(_out,RL_LearnNet_net_hidden_layers_size);
		OUT_VALUE(_out,RL_LearnNet_net_hidden_layers_count);
	}

	void print_to_file(const CString& _file_name) const 
	{
		std::tofstream out;
		out.open(_file_name);
		print(out);
	}
};
*/
