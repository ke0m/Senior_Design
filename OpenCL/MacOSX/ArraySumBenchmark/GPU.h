//
//  GPU.h
//  ArraySum1.1
//
//  Created by Joseph Jennings on 10/1/13.
//  Copyright (c) 2013 Joseph Jennings. All rights reserved.
//

#ifndef __ArraySum1_1__GPU__
#define __ArraySum1_1__GPU__


#include <stdlib.h>
#include <iostream>
#include <OpenCL/OpenCL.h>
#include <vector>

#endif /* defined(__ArraySum1_1__GPU__) */

class GPU{
    
private:
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    
    cl_int err;
    
    const char* fileName;
    const char* kernelName;
    
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    
    size_t work_units_per_kernel;
    
    int dataDims;
    
    cl_platform_id setPlatform();
    cl_device_id setDevice();
    cl_context createContext();
    size_t findProgramSize();
    char* createProgramBuffer();
    std::string getProgramInfo();
    cl_program buildProgram();
    cl_kernel createKernel();
    cl_command_queue createQueue();
    void setRunOnGPU(bool gpuBool);


public:


GPU(const char* kernelFileName, const char* nameOfKernel, bool gpuVCPU);

cl_platform_id getPlatform();
cl_device_id getDevice();
cl_context getContext();
cl_command_queue getQueue();
cl_kernel getKernel();
cl_program getProgram();
cl_int getErr();
cl_mem createFloatBuffer(int datadims);
void writeToDevice(cl_mem ddata, float* hdata, int datadims);
void writeVecToDevice(cl_mem ddata, std::vector<float> &vec);
void readFromDevice(cl_mem ddata, float* hdata, int datadims);
void readVecFromDevice(cl_mem ddata, std::vector<float> &vec);
void setGlobalKernelArg(cl_mem ddata, int kernelNum);
void setLocalKernelArg(cl_mem ddata, int kernelNum);
void setGlobalIntArg(int num, int kernelNum);
void executeKernel(int datadims);
void freeDeviceMem(cl_mem ddata);
void freeKernel();
void freeCommandQueue();
void freeProgram();
void freeContext();



};

