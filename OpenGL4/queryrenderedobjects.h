#pragma once

#include "utils.h"
#include "simple.h"

struct QueryRenderedObjects
{
protected:
	GLuint query_object_id;
	GLenum target;
	GLenum err_code;

public:
	QueryRenderedObjects()
		:query_object_id(0)
		,err_code(GL_NO_ERROR)
		,target(GL_SAMPLES_PASSED)
	{
	}

	~QueryRenderedObjects()
	{
		clear();
	}

	GLenum get_last_error() const {return err_code;}

	bool create()
	{
		clear();
		glGenQueries(1,&query_object_id);
		return !is_error(err_code);
	}

	void clear()
	{
		if(glIsQuery(query_object_id))
		{
			glDeleteQueries(1,&query_object_id);
			get_error(err_code);
		}
		query_object_id = 0;
	}

	/*
		_target = GL_SAMPLES_PASSED, GL_ANY_SAMPLES_PASSED, or
			GL_ANY_SAMPLES_PASSED_CONSERVATIVE
	*/
	bool pre_test_render(GLenum _target)
	{
		if(!prepare()) return false;
		target = _target;
		glBeginQuery(target,query_object_id);
		return !is_error(err_code);
	}

	bool post_test_render()
	{
		glEndQuery(target);
		return !is_error(err_code);
	}

	/*
		_mode = GL_QUERY_WAIT,
			GL_QUERY_NO_WAIT, GL_QUERY_BY_REGION_WAIT, or
			GL_QUERY_BY_REGION_NO_WAIT

		if used NO_WAIT mode then you may start drawing and 
		time by time check if results available if they will have became 
		available then you can stop or continue drawing. but if you should 
		continue drawing then you don`t lose time for waiting of results

		so after post_conditinal_render() continue drawing and block of data
		and time by time check is_result_available() to stop drawing or 
		to continue drawing. 
	*/
	bool pre_conditinal_render(GLenum _mode)
	{
		if(!prepare()) return false;
		glBeginConditionalRender(query_object_id,_mode);
		return !is_error(err_code);
	}

	bool post_conditinal_render()
	{
		glEndConditionalRender();
		return !is_error(err_code);
	}

	bool is_result_avalable() const 
	{
		GLint queryReady = 0;
		glGetQueryObjectiv(query_object_id, GL_QUERY_RESULT_AVAILABLE, &queryReady);
		return to_bool(queryReady);
	}

	GLint get_rendered_objects_count(bool _fast = true)
	{
		if(!glIsQuery(query_object_id)) return 1;

		bool result_avalable = false;
		GLint count = _fast?1:1000; /* counter to avoid a possible infinite loop */
		for(;count-- && !(result_avalable = is_result_avalable());) {}

		GLint samples = 0;
		if(result_avalable)
			glGetQueryObjectiv(query_object_id, GL_QUERY_RESULT, &samples);
		else
			samples = 1;

		return samples;
	}

protected:
	bool prepare()
	{
		if(!glIsQuery(query_object_id))
			return create();
		return true;
	}
};//struct QueryRenderedObjects