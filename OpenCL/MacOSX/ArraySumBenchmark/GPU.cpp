//
//  GPU.cpp
//  ArraySum1.1
//
//  Created by Joseph Jennings on 10/1/13.
//  Copyright (c) 2013 Joseph Jennings. All rights reserved.
//

#include "GPU.h"


#include <iostream>
#include <stdlib.h>
#include <OpenCL/OpenCL.h>
#include <sys/time.h>

cl_platform_id platform;
cl_device_id device;
cl_context context;
cl_command_queue queue;
cl_program program;
cl_kernel kernel;


//A cl_int used to store error flags that are returned if OpenCL function does not execute properly
cl_int err;
const char* fileName;
const char* kernelName;

FILE *program_handle;
char *program_buffer, *program_log;
size_t program_size, log_size;


//The number of work items in each dimension of the data.
size_t work_units_per_kernel;

bool runOnGPU = true;

cl_platform_id GPU::setPlatform(){
    
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS){
        std::cout << "Error: Failed to locate the platform." << std::endl;
        exit(1);
    }
    return platform;
}



cl_device_id GPU::setDevice(){
    if(runOnGPU == true){
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Failed to locate the device." << std::endl;
            exit(1);
            
        }
    }
    
    else if(runOnGPU == false){
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Failed to locate the device." << std::endl;
            exit(1);
            
        }
    }

    return device;
}


cl_context GPU::createContext(){
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create a context." << std::endl;
        exit(1);
    }
    
    return context;
}


size_t GPU::findProgramSize(){
    size_t pSize;
    if(!program_handle){
        std::cout << "Error: Failed to load Kernel" << std::endl;
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    pSize = ftell(program_handle);
    rewind(program_handle);
    return pSize;
}


char* GPU::createProgramBuffer(){
    program_size = findProgramSize();
    //program_handle = fopen("flopstestloop.cl", "r");
    program_buffer = (char*)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);
    
    return program_buffer;
}

std::string GPU::getProgramInfo(){
    std::string programLog;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    program_log = (char*)malloc(log_size+1);
    program_log[log_size] = '\0';
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size+1, program_log, NULL);
        
    programLog = program_log;   

    free(program_log);
    
    
    return programLog;
}

cl_program GPU::buildProgram(){
    program = clCreateProgramWithSource(context, 1, (const char **)&program_buffer, (const size_t *)&program_size, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the program" << std::endl;
        exit(1);
    }
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not compile the program" << std::endl;
        std::cout << getProgramInfo() << std::endl;
        exit(1);
    }
    
    return program;
    
}

cl_kernel GPU::createKernel(){
    kernel = clCreateKernel(program, "arraysum", &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the kernel." << std::endl;
        exit(1);
    }
    
    return kernel;
    
}

cl_command_queue GPU::createQueue(){
    queue = clCreateCommandQueue(context, device, 0, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the queue." << std::endl;
        exit(1);
    }
    
    return queue;
}

void GPU::setRunOnGPU(bool gpuBool){
    if(gpuBool == false){
        runOnGPU = false;
    }
}


GPU::GPU(const char* kernelFileName, const char* nameOfKernel, bool gpuVCPU){
    
    
    err = 0;
    fileName = kernelFileName;
    kernelName = nameOfKernel;
    program_handle = fopen(kernelFileName, "r");
    setRunOnGPU(gpuVCPU);
    
    
    
    platform = setPlatform();
    
    device = setDevice();
    
    context = createContext();
    
    program_buffer = createProgramBuffer();
    
    program = buildProgram();
    
    kernel = createKernel();
    
    queue = createQueue();
    
}


cl_platform_id GPU::getPlatform(){
    
    return platform;
}

cl_device_id GPU::getDevice(){
    
    return device;
}

cl_context GPU::getContext(){
    
    return context;
}

cl_command_queue GPU::getQueue(){
    
    return queue;
}

cl_kernel GPU::getKernel(){
    
    return kernel;
}

cl_program  GPU::getProgram(){
    
    return program;
}

cl_int GPU::getErr(){
    
    return err;
    
}


cl_mem GPU::createFloatBuffer(int datadims){
    cl_mem buff;
    buff = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*datadims, NULL, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the buffer." << std::endl;
        exit(1);
    }
    
    return buff;
    
}


void GPU::writeToDevice(cl_mem ddata, float* hdata, int datadims){
    err = clEnqueueWriteBuffer(queue, ddata, CL_TRUE, 0, sizeof(float)*datadims, hdata, 0, NULL, NULL);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not write to buffer." << std::endl;
        exit(1);
    }
    
}


void GPU::setGlobalKernelArg(cl_mem ddata, int kernelNum){
    err = clSetKernelArg(kernel, kernelNum, sizeof(cl_mem), &ddata);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
        exit(1);
    }
}

void GPU::setLocalKernelArg(cl_mem ddata, int kernelNum){
	err = clSetKernelArg(kernel, kernelNum, sizeof(cl_mem), NULL);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}
}

void GPU::executeKernel(int datadims){
    work_units_per_kernel = datadims;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &work_units_per_kernel, NULL, 0, NULL, NULL);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not execute the kernel." << std::endl;
        exit(1);
    }
}

void GPU::readFromDevice(cl_mem ddata, float* hdata, int datadims){
    err = clEnqueueReadBuffer(queue, ddata, CL_TRUE, 0, sizeof(float)*datadims, hdata, 0, NULL, NULL);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not read data from the kernel." << std::endl;
		std::cout << "OpenCL error code: " << std::endl;
        exit(1);
    }
    
}

void GPU::freeDeviceMem(cl_mem ddata){
    
    clReleaseMemObject(ddata);
}

void GPU::freeKernel(){
    
    clReleaseKernel(kernel);
}

void GPU::freeCommandQueue(){
    
    clReleaseCommandQueue(queue);
}

void GPU::freeProgram(){
    
    clReleaseProgram(program);
}

void GPU::freeContext(){
    
    clReleaseContext(context);
    
}


