//
//  ArraySumTotal.cpp
//  ArraySumBenchmark
//
//  Created by Joseph Jennings on 11/28/13.
//  Copyright (c) 2013 Joseph Jennings. All rights reserved.
//
//Formula to calculate Theoretical FLOPS: number of CUDA cores * clock speed of core * 2 (for FMA instruction set 2 FLOPS per cycle)

#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <CL\cl.h>
#include <string>
#include "Stopwatch.h"

int convertToString(const char *filename, std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if(f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);
		str = new char[size+1];
		if(!str)
		{
			f.close();
			return 0;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
		delete[] str;
		return 0;
	}
	std::cout<<"Error: failed to open file\n:"<<filename<<std::endl;
	return 1;
}

int main (int argc, const char * argv[])
{
 
#pragma region Declaration of Variables

    //Declaring Variables for for loops
    int minIters = 2;
    int maxIters = 16384;
    int minSize = 1000;
    int maxSize = 8000;

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
	cl_uint num_platforms;
    

	//writing flops data to a file
	std::ofstream outputfile1;
	std::ofstream outputfile2;
	std::ofstream gpuvalues;
	std::ofstream cpuvalues;
	outputfile1.open("..\\flops_results_files\\multicpubiggerarraysize_lessitersmore.txt");
	outputfile2.open("..\\flops_results_files\\test.txt");

    //A cl_int used to store error flags that are returned if OpenCL function does not execute properly
    cl_int err;
    const char* fileName;
    const char* kernelName;
    
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    std::string programLog;
    
    //The number of work items in each dimension of the data.
    size_t work_units_per_kernel_vec;
	size_t work_units_per_kernel;
    
    Stopwatch sw;
	bool checkSum = true;
    
    
    //Outer most loop: This loop should encompass all code and is used in order to loop over
    //different values of the iterations and array size to create the data for the plot.
	for(int i = minSize; i < maxSize; i*=sqrt(2)){
			for(int j = minIters; j < maxIters; j*=2){

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

					h_xx[x][y] = 1;
					h_yy[x][y] = 1;
                    
				}
                
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

#pragma endregion

			//Here the benchmark occurs. Within this while loop must occurr all OpenCL calls and executions

		#pragma region OpenCL platform setup, device call, kernel build, etc.

			err = clGetPlatformIDs(1, &platform, &num_platforms);
			if (err != CL_SUCCESS){
				std::cout << "Error: Failed to locate the platform." << std::endl;
				system("pause");
				exit(1);
			}


			err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Failed to locate the device." << std::endl;
				std::cout << "OpenCL Error code: " << err << std::endl;
				system("pause");
				exit(1);
                    
			}
			context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create a context." << std::endl;
				system("pause");
				exit(1);
			}

			const char* fileName = "winflopf16memfma.cl";
			std::string sourceStr;
			int status = convertToString(fileName, sourceStr);
			const char *source = sourceStr.c_str();
			size_t sourceSize[] = {strlen(source)};
			program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the program" << std::endl;
				system("pause");
				exit(1);
			}
                
			err = clBuildProgram(program, 1, &device, "-cl-strict-aliasing -cl-unsafe-math-optimizations -cl-finite-math-only -cl-fast-relaxed-math -DMAC" , NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not compile the program" << std::endl;
				clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
				program_log = (char*)malloc(log_size+1);
				program_log[log_size] = '\0';
				clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size+1, program_log, NULL);
				programLog = program_log;
				std::cout << programLog << std::endl;
				free(program_log);
				system("pause");
				exit(1);
			}
                
			kernel = clCreateKernel(program, "arraysum", &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the kernel." << std::endl;
				system("pause");
				exit(1);
			}
                
			queue = clCreateCommandQueue(context, device, 0, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the queue." << std::endl;
				system("pause");
				exit(1);
			}
                
			cl_mem d_xx, d_yy, d_zz;
                
			d_xx = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				system("pause");
				exit(1);
			}

			d_yy = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*dims, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				system("pause");
				exit(1);
			}

			
			d_zz = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float)*dims, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				std::cout << "OpenCL Error Code: " << err << std::endl;
				system("pause");
				exit(1);
			}

                
			//pakcing of the arrays goes here
            
			//copy the data to the GPU
			err = clEnqueueWriteBuffer(queue, d_xx, CL_FALSE, 0, sizeof(float)*dims, h_xx1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not write to buffer." << std::endl;
				system("pause");
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

			err = clSetKernelArg(kernel, 4, sizeof(dims), &dims);
			if(err != CL_SUCCESS){
				std::cout << "Error: could not set the integer kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				system("pause");
				exit(1);
			}

		#pragma endregion

		#pragma region OpenCL and C++ Benchmarks
                

			int maxTime = 5;
			int gpucount = 0;
			sw.restart();
			while (sw.getTime() < maxTime){
				// I should modify this number to see if I cannot acheive better performance.
				size_t local_group_size = 1024; //threads per block
				size_t global_group_size = ceil((dims/local_group_size) + 1) * local_group_size;
				size_t global_group_size_vec = ceil((dims/local_group_size/16) + 1) * local_group_size;
				//printf("Global Work Group size: %d \t Local Work Group Size: %d\n" , global_group_size, local_group_size);
				work_units_per_kernel_vec = ((dims)/16 + 1);
				work_units_per_kernel = dims;
				err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_group_size_vec, &local_group_size, 0, NULL, NULL);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not execute the kernel." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					system("pause");
					exit(1);
				}

				clFinish(queue);

			gpucount++;
				
			}

			sw.stop();
			float gputime = sw.getTime();


			err = clEnqueueReadBuffer(queue, d_zz, CL_TRUE, 0, sizeof(float)*dims, h_zz1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not read data from the kernel." << std::endl;
				std::cout << "OpenCL error code: " << std::endl;
				exit(1);
			}
                
			//unpacking of the array goes here
            	

			//Freeing up memory. Hopefully!
			clReleaseMemObject(d_xx);
			clReleaseMemObject(d_yy);
			clReleaseMemObject(d_zz);
			clReleaseKernel(kernel);
			clReleaseCommandQueue(queue);
			clReleaseProgram(program);
			clReleaseContext(context);

			int cpucount = 0;
			sw.restart();
			//here the timing ends
			while(sw.getTime() < maxTime)
			{
				for(int x = 0; x < n1; x++)
				{
					for(int y = 0; y < n2; y++)
					{
						h_zz[x][y] = h_xx[x][y]*h_xx[x][y] + h_yy[x][y];
						//h_zz[x][y] = h_xx[x][y] + h_yy[x][y];
						for(int z = 0; z < j; z++)
						{
							h_zz[x][y] += h_yy[x][y]*h_yy[x][y];
							//h_zz[x][y] += h_yy[x][y];
						}
							
					}
				}

				cpucount++;
			}
			sw.stop();
			float cputime = sw.getTime();

		#pragma endregion


			#pragma region Check Sum

			for(int x = 0; x < n1; x++)
			{
				for(int y = 0; y < n2; y++)
				{
					if(h_zz1[x * n2 + y] == h_zz[x][y]) //check this offset to be sure
					{
						//printf("GPU Value: %f\n", h_zz1[x * n2 + y]);
						//printf("CPU Value: %f\n", h_zz[x][y]);
					}
					else
					{
						checkSum = false;
						printf("Check Sum failed at array element (%d, %d)\n", x, y);
						printf("GPU Value: %f\n", h_zz1[x * n2 + y]);
						printf("CPU Value: %f\n", h_zz[x][y]);
						//system("pause");
						//exit(1);
					}
				}
			
				std::cout << std::endl;
			}
					
			if(checkSum == true)
				std::cout << "Check Sum Passed!" << std::endl << std::endl;
			else
				std::cout << "Check Sum Failed!" << std::endl << std::endl;

			#pragma endregion

	#pragma region Write Calculations to file

			//gpuvalues.open("..\\check_sum_result_files\\gpuvalues.txt");
			//cpuvalues.open("..\\check_sum_result_files\\cpuvalues.txt");
			//for(int x = 0; x < n1; x++)
			//{
			//	for(int y = 0; y < n2; y++)
			//	{
			//		gpuvalues << h_zz1[x * n2 + y] << std::endl;
			//		cpuvalues << h_zz[x][y] << std::endl;
			//	}
			//}
			//
			//gpuvalues.close();
			//cpuvalues.close();

	#pragma endregion


			#pragma region free memory and display results

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
			float gpucountf = (float) gpucount;
			float cpucountf = (float) cpucount;

			std::cout << "Iters: " << iters << std::endl;
			std::cout << "n1: " << n1f << std::endl;
			std::cout << "n2: " << n2f << std::endl;
			//std::cout << "Local Group Size: " << grpsize << std::endl;
			std::cout << "GPUcount: " << gpucountf << std::endl;
			std::cout << "CPUcount: " << cpucountf << std::endl;
			std::cout << "GPUTime: " << gputime << std::endl;
			std::cout << "CPUTime: " << cputime << std::endl;
            
			float gpumflops = 2*n1f*n2f*gpucountf*iters*1.0e-06/gputime;
			float cpumflops = 2*n1f*n2f*cpucountf*iters*1.0e-06/cputime;
            
			//std::cout << "Number of MegaFLOPs: " << n1f*n2f*500*countf*1.0e-6 << std::endl;
			std::cout << gpumflops << " GPU MegaFLOPS" << std::endl;
			std::cout << cpumflops << " CPU MegaFLOPS" << std::endl;
			std::cout << std::endl;

			outputfile1 << gpumflops << std::endl;
			outputfile2 << cpumflops << std::endl;

        #pragma endregion

		}
        std::cout << std::endl;
    }

	outputfile1.close();
	outputfile2.close();
    return 0;
}
