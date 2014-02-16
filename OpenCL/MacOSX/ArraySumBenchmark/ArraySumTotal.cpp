//
//  ArraySumTotal.cpp
//  ArraySumBenchmark
//
//  Created by Joseph Jennings on 11/28/13.
//  Copyright (c) 2013 Joseph Jennings. All rights reserved.
//


#include <iostream>
#include <stdlib.h>
#include <OpenCL/OpenCL.h>
#include "Stopwatch.h"


int main (int argc, const char * argv[]){
 
    
    //Declaring Variables for for loops
    int minIters = 2;
    int maxIters = 32768;
    int minSize = 500;
    int maxSize = 2500;
    int a = 0;
    int b = 0;
    
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
    std::string programLog;
    
    //The number of work items in each dimension of the data.
    size_t work_units_per_kernel;
    
    Stopwatch sw;
    
    
    //Outer most loop: This loop should encompass all code and is used in order to loop over
    //different values of the iterations and array size to create the data for the plot.
    
    for(int i = minSize; i < maxSize; i+=100){
        std::cout << "Array Size: " << i << std::endl;
        for(int j = minIters; j < maxIters; j*=2){
            std::cout << "Iterations: " << j << std::endl;
            
            //TODO: Move lower in the code
            //data[a][b] = datagpu.runGPUBenchmark(10, 3, 3);
            
            //Initializing the arrays
            
            int n1 = i;
            int n2 = i+1;
            int iters = j;
            long dims = n1*n2;
            
            float **h_xx = new float*[n1];
            float **h_yy = new float*[n1];
            float **h_zz = new float*[n1];
            
            for(int x = 0; x<n1; x++){
                
                h_xx[x] = new float [n2];
                h_yy[x] = new float [n2];
                h_zz[x] = new float [n2];
                
                //Initializing the arrays.
                for(int y = 0; y<n2; y++){

                    h_xx[x][y] = x+y;
                    h_yy[x][y] = x+y;
                    
                }
                
            }
            
            //Here the benchmark occurs. Within this while loop must occurr all OpenCL calls and executions
            int maxTime = 5;
            int count = 0;
            sw.restart();
            while (sw.getTime() < maxTime){
                
                err = clGetPlatformIDs(1, &platform, NULL);
                if (err != CL_SUCCESS){
                    std::cout << "Error: Failed to locate the platform." << std::endl;
                    exit(1);
                }
                err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Failed to locate the device." << std::endl;
                    exit(1);
                    
                }
                context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not create a context." << std::endl;
                    exit(1);
                }

                
                program_handle = fopen("floptmem.cl", "r");
                if(!program_handle){
                    std::cout << "Error: Failed to load Kernel" << std::endl;
                    exit(1);
                }
                fseek(program_handle, 0, SEEK_END);
                program_size = ftell(program_handle);
                rewind(program_handle);
                program_buffer = (char*)malloc(program_size + 1);
                program_buffer[program_size] = '\0';
                fread(program_buffer, sizeof(char), program_size, program_handle);
                fclose(program_handle);
                
                program = clCreateProgramWithSource(context, 1, (const char **)&program_buffer, (const size_t *)&program_size, &err);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not create the program" << std::endl;
                    exit(1);
                }
            
                err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not compile the program" << std::endl;
                    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
                    program_log = (char*)malloc(log_size+1);
                    program_log[log_size] = '\0';
                    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size+1, program_log, NULL);
                    programLog = program_log;
                    std::cout << programLog << std::endl;
                    free(program_log);
                    exit(1);
                }
                
                kernel = clCreateKernel(program, "arraysum", &err);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not create the kernel." << std::endl;
                    exit(1);
                }
                
                queue = clCreateCommandQueue(context, device, 0, &err);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not create the queue." << std::endl;
                    exit(1);
                }
                
                cl_mem d_xx, d_yy, d_zz;
                
                d_xx = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*dims, NULL, &err);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not create the buffer." << std::endl;
                    exit(1);
                }

                d_yy = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*dims, NULL, &err);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not create the buffer." << std::endl;
                    exit(1);
                }

                d_zz = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*dims, NULL, &err);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not create the buffer." << std::endl;
                    exit(1);
                }
                
                float *h_xx1 = new float[dims];
                float *h_yy1 = new float[dims];
                float *h_zz1 = new float[dims];
                
                
                //packing arrays
                int k = 0;
                for (int x = 0; x < n1; x++){
                    for (int y = 0; y < n2; y++){
                        h_xx1[k] = h_xx[x][y];
                        h_yy1[k] = h_yy[x][y];
                        k++;
                    }
                    
                }
            
                err = clEnqueueWriteBuffer(queue, d_xx, CL_FALSE, 0, sizeof(float)*dims, h_xx1, 0, NULL, NULL);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not write to buffer." << std::endl;
                    exit(1);
                }
                err = clEnqueueWriteBuffer(queue, d_yy, CL_FALSE, 0, sizeof(float)*dims, h_yy1, 0, NULL, NULL);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not write to buffer." << std::endl;
                    exit(1);
                }
                
                
                err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_xx);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not set the kernel argument." << std::endl;
                    std::cout << "OpenCL error code: " << err << std::endl;
                    exit(1);
                }
                
                
                err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_yy);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not set the kernel argument." << std::endl;
                    std::cout << "OpenCL error code: " << err << std::endl;
                    exit(1);
                }
                
                err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_zz);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not set the kernel argument." << std::endl;
                    std::cout << "OpenCL error code: " << err << std::endl;
                    exit(1);
                }
                
                err = clSetKernelArg(kernel, 3, sizeof(j), &j);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not set the integer kernel argument." << std::endl;
                    std::cout << "OpenCL error code: " << err << std::endl;
                    exit(1);
                }
                
                
                work_units_per_kernel = dims;
                err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &work_units_per_kernel, NULL, 0, NULL, NULL);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not execute the kernel." << std::endl;
                    exit(1);
                }

    
                
                err = clEnqueueReadBuffer(queue, d_zz, CL_TRUE, 0, sizeof(float)*dims, h_zz1, 0, NULL, NULL);
                if(err != CL_SUCCESS){
                    std::cout << "Error: Could not read data from the kernel." << std::endl;
                    std::cout << "OpenCL error code: " << std::endl;
                    exit(1);
                }
                
                //unpacking the 1D array
                k = 0;
                for (int x = 0; x < n1; x++){
                    for (int y = 0; y < n2; y++){
                        h_zz[x][y] = h_zz1[k];
                        k++;
                    }
                }
                
                delete [] h_xx1;
                delete [] h_yy1;
                delete [] h_zz1;
      
            	count++;

                //Freeing up memory. Hopefully!
                clReleaseMemObject(d_xx);
                clReleaseMemObject(d_yy);
                clReleaseMemObject(d_zz);
                clReleaseKernel(kernel);
                clReleaseCommandQueue(queue);
                clReleaseProgram(program);
                clReleaseContext(context);
                

            }

            sw.stop();

            for (int x = 0; x < n1; x++)
			{
                delete [] h_xx[x];
                delete [] h_yy[x];
                delete [] h_zz[x];
            }
            
            delete [] h_xx;
            delete [] h_yy;
            delete [] h_zz;
            

            
            float n1f = (float) n1;
            float n2f = (float) n2;
            float countf = (float) count;

			
			std::cout << "n1: " << n1f << std::endl;
			std::cout << "n2: " << n2f << std::endl;
			std::cout << "Iters: " << iters << std::endl;
			std::cout << "count: " << countf << std::endl;
			std::cout << "Time: " << sw.getTime() << std::endl;
			std::cout << std::endl;
            
            float mflops = n1f*n2f*countf*iters*1.0e-06/sw.getTime();
            
            //std::cout << "Number of MegaFLOPs: " << n1f*n2f*500*countf*1.0e-6 << std::endl;
            std::cout << mflops << " MegaFLOPS" << std::endl;

            b++;
        }
        a++;
        std::cout << std::endl;
    }
    
    
    return 0;
}
