#pragma once 

#include "utils.h"

namespace opencl
{
	struct DLNetKernels;
	struct DLNet;

	struct Environment
	{
	protected:
		std::vector<cl::Platform> platforms;
		cl::Platform platform;	
		cl::Context context;
		std::vector<cl::Device> devices;
		cl::Device device;
		cl::CommandQueue queue;
		cl::Program program;

	public:
		Environment()
		{
		}

		void select_platform(size_t _platform_idx)
		{
			cl::Platform::get(&platforms);

			if(_platform_idx>=platforms.size()) return;

			platform = platforms[_platform_idx];
		}

		void select_device(cl_device_type _device_type,size_t _device_idx)
		{
			cl_context_properties cprops[] = {
				CL_CONTEXT_PLATFORM
				,(cl_context_properties)(platform())
				,0
			};
			context = cl::Context(_device_type, cprops);
			devices = context.getInfo<CL_CONTEXT_DEVICES>();
			if(_device_idx>=devices.size()) return;
			device = devices[_device_idx];

			queue = cl::CommandQueue(context,device,0);
		}

		bool build_program(const char* _code,cl_uint& _status,std::string& _log)
		{
			try
			{
				cl::Program::Sources src(
					1
					,std::make_pair(_code,0)
					);
				program = cl::Program(context,src);
				program.build(devices);
			}
			catch(cl::Error& _err)
			{
				if(_err.err()==CL_BUILD_PROGRAM_FAILURE)
				{
					_status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device());
					_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device());
				}
				return false;
			}
			return true;
		}

		cl::Kernel get_kernel(const char* _name)
		{
			return cl::Kernel(program,_name);
		}

		cl::CommandQueue& get_queue()
		{
			return queue;
		}

		cl::Context& get_context()
		{
			return context;
		}

		friend struct DLNetKernels;
		friend struct DLNet;
	};//struct Environment
}

