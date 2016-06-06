#pragma once

//#define CL_HPP_NO_STD_VECTOR // Use cl::vector instead of STL version
#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <SDL.h>
#undef main
#include <SDL_video.h>
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
	cl::Context context(CL_DEVICE_TYPE_CPU, contextDesc, NULL, NULL, &err);
	checkErr(err, "Context creation");

	// Get devices
	cl::vector<cl::Device> devices;
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	checkErr(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

	// Select device
	cl::Device& selectedDevice = devices[0];

	int resX = 640;
	int resY = 480;
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
	err = kernel.setArg(0, outCL);
	checkErr(err, "Kernel::setArg()");

	// Run kernel
	cl::CommandQueue queue(context, selectedDevice, 0, &err);
	checkErr(err, "CommandQueue::CommandQueue()");
	cl::Event event;
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(resX, resY), cl::NDRange(1, 1), NULL, &event);
	checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");

	// Wait kernel
	event.wait();

	// Read back result from GPU memory to CPU
	err = queue.enqueueReadBuffer(outCL, CL_TRUE, 0, resX * resY * bytesPerPixel, backbuffer);
	checkErr(err, "ComamndQueue::enqueueReadBuffer()");

	
	// Init SDL
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		assert(0);
	}

	// Create window
	SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *MainWindow = SDL_CreateWindow("GPGPU",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        resX, resY,
        SDL_WINDOW_SHOWN
        );

    SDL_Renderer *renderer = SDL_CreateRenderer(MainWindow, -1, 0);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


    SDL_Texture* screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, resX, resY);

	while (1) 
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{

		}

		{
			// Initialize texture pixels to a red opaque RGBA value
			unsigned char* bytes = nullptr;
			int pitch = 0;
			SDL_LockTexture(screenTexture, nullptr, reinterpret_cast<void**>(&bytes), &pitch);
			memcpy(bytes, backbuffer, resX * resY * bytesPerPixel);
			SDL_UnlockTexture(screenTexture);

			//SDL_Rect destination = { 0, 0, resX, resY };
			SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}
	}

    //Clean up
    SDL_DestroyTexture(screenTexture);
    SDL_DestroyWindow(MainWindow);
    SDL_Quit();
}