#pragma once

//#define CL_HPP_NO_STD_VECTOR // Use cl::vector instead of STL version
#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <SDL.h>
#undef main
#include <SDL_video.h>
#include <SDL_opengl.h>
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
#include <chrono>
using namespace std::chrono;

const std::string hw("Hello World\n");

const char *getErrorString(cl_int error)
{
switch(error){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}

void checkErr(cl_int err, const char * name)
{
	if (err != CL_SUCCESS)
	{
		std::cerr << "ERROR: " << name << " (" << getErrorString( err ) << ")" << std::endl;
		assert(0);
	}
}

void ricsi_hello()
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
	cl::Context context(CL_DEVICE_TYPE_GPU, contextDesc, NULL, NULL, &err);
	checkErr(err, "Context creation");

	// Get devices
	cl::vector<cl::Device> devices;
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

	// Select device
	cl::Device& selectedDevice = devices[0];

	int resX = 64 * 20;
	int resY = 64 * 10;
	int bytesPerPixel = 3;

	// Allocate memory
	uint8_t* backbuffer = new uint8_t[resX * resY * bytesPerPixel];
	cl::Buffer outCL(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, (size_t)(resX * resY * bytesPerPixel), backbuffer, &err);
	checkErr(err, "Buffer::Buffer()");

	//Compile kernel program
	std::ifstream file("Ricsi/hello.cl");
	checkErr(file.is_open() ? CL_SUCCESS:-1, "Ricsi/hello.cl");
	std::string prog(std::istreambuf_iterator<char>(file),(std::istreambuf_iterator<char>()));
	cl::Program::Sources source;
	source.push_back(prog);
	cl::Program program(context, source);
	err = program.build(devices,"-x clc++");
	checkErr(err, "Program::build()");

	// Create kernel
	cl::Kernel kernel(program, "hello", &err);
	checkErr(err, "Kernel::Kernel()");

	
	// Init SDL
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		assert(0);
	}

	cl::CommandQueue queue(context, selectedDevice, 0, &err);
	checkErr(err, "CommandQueue::CommandQueue()");
	cl::Event eventt;


	//Use OpenGL 3.1 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Initialize video subsystem
	SDL_Window* window = NULL;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        // Display error message
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    else
    {
        // Create window
		window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resX, resY, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            // Display error message
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            return;
        }
        else
        {
            // Create OpenGL context
			SDL_GLContext  glContext = SDL_GL_CreateContext(window);

            if( glContext == NULL )
            {
                // Display error message
                printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
                return;
            }
            else
            {
                // Initialize glew
                //glewInit();
            }
        }
    }

	glEnable(GL_TEXTURE_2D);
	GLuint gen;
	glGenTextures( 1, &gen);
	glBindTexture(GL_TEXTURE_2D, gen);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	while (1) 
	{
		// Start measure
		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
		
		}

		// Set args
		err = kernel.setArg(0, outCL);
		checkErr(err, "Kernel::setArg()");

		// Run kernel
		err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(resX, resY), cl::NDRange(16,16), NULL, &eventt);
		checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");

		// Wait kernel
		eventt.wait();
		
		// Read back result from GPU memory to CPU
		err = queue.enqueueReadBuffer(outCL, CL_TRUE, 0, resX * resY * bytesPerPixel, backbuffer);
		checkErr(err, "ComamndQueue::enqueueReadBuffer()");
		
		// TODO VERY SLOW
		// Initialize texture pixels to a red opaque RGBA value
		//unsigned char* bytes = nullptr;
		//int pitch = 0;
		//SDL_LockTexture(screenTexture, nullptr, reinterpret_cast<void**>(&bytes), &pitch);
		//memcpy(bytes, backbuffer, resX * resY * bytesPerPixel);
		//SDL_UnlockTexture(screenTexture);
		//
		////SDL_Rect destination = { 0, 0, resX, resY };
		//SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
		//SDL_RenderPresent(renderer);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, resX, resY, 0, GL_RGB, GL_UNSIGNED_BYTE, backbuffer);

		glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex3f(-1,-1,-1);
		glTexCoord2f(0,0);
		glVertex3f(-1,1,-1);
		glTexCoord2f(1,0);
		glVertex3f(1,1,-1);
		glTexCoord2f(1,1);
		glVertex3f(1,-1,-1);
		glEnd();
		//glLoadIdentity();
		

		// Update window with OpenGL rendering
		SDL_GL_SwapWindow(window);

		// End measure
		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>( t2 - t1 ).count();

		std::string str = std::to_string(duration / 1000.0);
		SDL_SetWindowTitle(window, str.c_str());
	}

    //Clean up
   // SDL_DestroyTexture(screenTexture);
    SDL_DestroyWindow(window);
    SDL_Quit();
}