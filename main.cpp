//#define CL_HPP_NO_STD_VECTOR // Use cl::vector instead of STL version
#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <CL/cl2.hpp>
#include <utility>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <cassert>
#include <string>
#include <windows.h>

const std::string hw("Hello World\n");


void checkErr(cl_int err, const char * name)
{
	if (err != CL_SUCCESS)
	{
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
}

int main()
{
	cl_int err;

	// Get available platforms
	cl::vector< cl::Platform > platforms;
	cl::Platform::get( &platforms );

	checkErr(platforms.size() > 0 ? CL_SUCCESS : -1, "platforms.size() > 0");

	// Select platform
	cl::Platform& selectedPlatform = platforms[0];

	// Print available "platforms"
	std::cout<<platforms.size() << " devices found\n";
	for(int i = 0; i < platforms.size(); ++i )
	{
		std::string vendor;
		std::string name;
		platforms[i].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &vendor);
		platforms[i].getInfo((cl_platform_info)CL_PLATFORM_NAME, &name);

		std::cout<<"Device "<<i<<". ( " << name <<" , " << vendor << " )\n";
	}
	std::puts("");

	// Create context
	cl_context_properties contextDesc[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(selectedPlatform)(), 0 }; 
	cl::Context context(CL_DEVICE_TYPE_CPU, contextDesc, NULL, NULL, &err);
	checkErr(err, "Context creation");

	// Get devices
	cl::vector<cl::Device> devices;
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

	// Select device
	cl::Device& selectedDevice = devices[0];

	// Allocate memory
	char * outH = new char[hw.length()+1];
	cl::Buffer outCL(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, hw.length() + 1, outH, &err);
	checkErr(err, "Buffer::Buffer()");

	//Compile kernel program
	std::ifstream file("hello.cl");
	checkErr(file.is_open() ? CL_SUCCESS:-1, "hello.cl");
	std::string prog(std::istreambuf_iterator<char>(file),(std::istreambuf_iterator<char>()));
	cl::Program::Sources source;
	source.push_back(prog);
	cl::Program program(context, source);
	err = program.build(devices,"");
	checkErr(err, "Program::build()");

	// Create kernel
	cl::Kernel kernel(program, "hello", &err);
	checkErr(err, "Kernel::Kernel()");
	err = kernel.setArg(0, outCL);
	checkErr(err, "Kernel::setArg()");

	// Run kernel
	cl::CommandQueue queue(context, selectedDevice, 0, &err);
	checkErr(err, "CommandQueue::CommandQueue()");
	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(hw.length() + 1), cl::NDRange(1, 1), NULL,&event);
	checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");

	// Wait kernel
	event.wait();

	// Read back result from GPU memory to CPU
	err = queue.enqueueReadBuffer(outCL, CL_TRUE, 0, hw.length() + 1, outH);
	checkErr(err, "ComamndQueue::enqueueReadBuffer()");
	std::cout << outH;

	// Print result
	int tmp;
	std::cin >> tmp;
	return EXIT_SUCCESS;
}