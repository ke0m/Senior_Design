#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <CL\cl.h>
#include <string>
#include <math.h>
#include "Stopwatch.h"

#define RAND_MAX 10000

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

#pragma region Variable Definition

	const int n1 = 2048;
	const int n2 = n1+2;// MUST be even
	const int dims = n1*n2;
	int value = 0;

	cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel0, kernel1;
	cl_uint num_platforms;


    cl_int err;
    const char* fileName;
    const char* kernelName;

	FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    std::string programLog;

	std::ofstream inputData;
	std::ofstream cpuOutput;
	std::ofstream gpuOutput;
	std::ofstream perfData;
	cpuOutput.open("cpuoutput.txt");
	gpuOutput.open("gpuoutput.txt");
	inputData.open("inputData.txt");
	perfData.open("perfplot.txt");

	Stopwatch sw;

	float e11, e12, e22;
	float r00, r01, r10, r11;
	float rs, ra, rb, sa, sb;
	float r1, r2, s1, s2;
	float alpha = 18.0f;

	int programType;

#pragma endregion

#pragma region OpenCL Setup

	err = clGetPlatformIDs(1, &platform, &num_platforms);
	if (err != CL_SUCCESS){
		std::cout << "Error: Failed to locate the platform." << std::endl;
		system("pause");
		exit(1);
	}


	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
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

	fileName = "ref_sosmoothing.cl";
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
                
	err = clBuildProgram(program, 1, &device, "-cl-strict-aliasing -cl-unsafe-math-optimizations -cl-finite-math-only -cl-fast-relaxed-math -DMAC", NULL, NULL); //here I use the cl-opt-disable flag to diable the FMA rounding. This was the only way I could get both CPU and GPU output to match
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
           
	kernel0 = clCreateKernel(program, "setMem", &err);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not create the kernel." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		system("pause");
		exit(1);
	}

	kernel1 = clCreateKernel(program, "soSmoothingNew", &err);  //first kernel I wrote. It made sense to me to write it this way but it is hard to read
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not create the kernel." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		system("pause");
		exit(1);
	}
                
	queue = clCreateCommandQueue(context, device, 0, &err);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not create the queue." << std::endl;
		system("pause");
		exit(1);
	}


#pragma endregion

#pragma Initializing and Packing the arrays


	float **h_r   = new float*[n1];
	float **h_s   = new float*[n1];
	float **s     = new float*[n1];
    float **s_p    = new float*[n1];
	float **h_d11 = new float*[n1];
	float **h_d12 = new float*[n1];
	float **h_d22 = new float*[n1];

	for(int i = 0; i < n1; i++){
                
		h_r[i]   = new float [n2];
		h_s[i]   = new float [n2];
		s[i]     = new float [n2];
        s_p[i]   = new float [n2];
		h_d11[i] = new float [n2];
		h_d12[i] = new float [n2];
		h_d22[i] = new float [n2];

                
		//Initializing the arrays.
		for(int j = 0; j < n2; j++){

			h_r[i][j]   = (float)rand()/(float)RAND_MAX;
			h_d11[i][j] = (float)rand()/(float)RAND_MAX;
			h_d12[i][j] = (float)rand()/(float)RAND_MAX;
			h_d22[i][j] = (float)rand()/(float)RAND_MAX;
			h_s[i][j]   = 0.0f;
			s[i][j]     = 0.0f;
            s_p[i][j]   = 0.0f;
                    
		}
                
	}

		float *h_r1 = new float[dims];
		float *h_s1 = new float[(n1+1)*(n2+1)];
		float *h_d11_1 = new float[dims];
        float *h_d12_1 = new float[dims];
		float *h_d22_1 = new float[dims];
                
		//packing arrays
		int k = 0;
		for (int i = 0; i < n1; i++){
			for (int j = 0; j < n2; j++){
				h_r1[k]   = h_r[i][j];
				h_s1[k]   = h_s[i][j];
				h_d11_1[k] = h_d11[i][j];
				h_d12_1[k] = h_d12[i][j];
				h_d22_1[k] = h_d22[i][j];
				k++;
			}
                    
		}



#pragma endregion


#pragma region User Input

		std::cout << "Please type: (1) 'Image' (2) 'Benchmark' (3) 'PerfPlot' ";
		std::cin >> programType;

#pragma endregion

#pragma region SOS Image Creation



		if(programType == 1)
		{

			//SOS CPU Image
			for(int i2 = 1; i2 < n1; ++i2)
			{
				for(int i1 = 1; i1 < n2; ++i1)
				{

					e11 = alpha * h_d11[i2][i1]; //fill matrices here with random numbers
					e12 = alpha * h_d12[i2][i1]; //fill D matrix with random numbers
					e22 = alpha * h_d22[i2][i1];
					r00 = h_r[i2  ][i1  ];
					r01 = h_r[i2  ][i1-1];
					r10 = h_r[i2-1][i1  ];
					r11 = h_r[i2-1][i1-1];
					rs = 0.25f*(r00+r01+r10+r11);
					ra = r00-r11;
					rb = r01-r10;
					r1 = ra-rb;
					r2 = ra+rb;
					s1 = e11*r1+e12*r2;
					s2 = e12*r1+e22*r2;
					sa = s1+s2;
					sb = s1-s2;
					s[i2  ][i1  ] += sa+rs;
					s[i2  ][i1-1] -= sb-rs;
					s[i2-1][i1  ] += sb+rs;
					s[i2-1][i1-1] -= sa-rs;

				}
			}
	
		
			//SOS CPU Prepare Parallel
			for(int i2 = 1; i2 < n1-1; i2 += 2)
			{
				for(int i1 = 1; i1 < n2-1; i1 += 2)
				{

					e11 = alpha * h_d11[i2][i1]; //fill matrices here with random numbers
					e12 = alpha * h_d12[i2][i1]; //fill D matrix with random numbers
					e22 = alpha * h_d22[i2][i1];
					r00 = h_r[i2  ][i1  ];
					r01 = h_r[i2  ][i1-1];
					r10 = h_r[i2-1][i1  ];
					r11 = h_r[i2-1][i1-1];
					rs = 0.25f*(r00+r01+r10+r11);
					ra = r00-r11;
					rb = r01-r10;
					r1 = ra-rb;
					r2 = ra+rb;
					s1 = e11*r1+e12*r2;
					s2 = e12*r1+e22*r2;
					sa = s1+s2;
					sb = s1-s2;
					s_p[i2  ][i1  ] += sa+rs;
					s_p[i2  ][i1-1] -= sb-rs;
					s_p[i2-1][i1  ] += sb+rs;
					s_p[i2-1][i1-1] -= sa-rs;

				}
			}

			for(int i2 = 1; i2 < n1; i2 += 2)
			{
				for(int i1 = 2; i1 < n2; i1 += 2)
				{

					e11 = alpha * h_d11[i2][i1]; //fill matrices here with random numbers
					e12 = alpha * h_d12[i2][i1]; //fill D matrix with random numbers
					e22 = alpha * h_d22[i2][i1];
					r00 = h_r[i2  ][i1  ];
					r01 = h_r[i2  ][i1-1];
					r10 = h_r[i2-1][i1  ];
					r11 = h_r[i2-1][i1-1];
					rs = 0.25f*(r00+r01+r10+r11);
					ra = r00-r11;
					rb = r01-r10;
					r1 = ra-rb;
					r2 = ra+rb;
					s1 = e11*r1+e12*r2;
					s2 = e12*r1+e22*r2;
					sa = s1+s2;
					sb = s1-s2;
					s_p[i2  ][i1  ] += sa+rs;
					s_p[i2  ][i1-1] -= sb-rs;
					s_p[i2-1][i1  ] += sb+rs;
					s_p[i2-1][i1-1] -= sa-rs;

				}
			}

			for(int i2 = 2; i2 < n1; i2 += 2)
			{
				for(int i1 = 1; i1 < n2; i1 += 2)
				{

					e11 = alpha * h_d11[i2][i1]; //fill matrices here with random numbers
					e12 = alpha * h_d12[i2][i1]; //fill D matrix with random numbers
					e22 = alpha * h_d22[i2][i1];
					r00 = h_r[i2  ][i1  ];
					r01 = h_r[i2  ][i1-1];
					r10 = h_r[i2-1][i1  ];
					r11 = h_r[i2-1][i1-1];
					rs = 0.25f*(r00+r01+r10+r11);
					ra = r00-r11;
					rb = r01-r10;
					r1 = ra-rb;
					r2 = ra+rb;
					s1 = e11*r1+e12*r2;
					s2 = e12*r1+e22*r2;
					sa = s1+s2;
					sb = s1-s2;
					s_p[i2  ][i1  ] += sa+rs;
					s_p[i2  ][i1-1] -= sb-rs;
					s_p[i2-1][i1  ] += sb+rs;
					s_p[i2-1][i1-1] -= sa-rs;

				}
			}

			for(int i2 = 2; i2 < n1; i2 += 2)
			{
				for(int i1 = 2; i1 < n2; i1 += 2)
				{

					e11 = alpha * h_d11[i2][i1]; //fill matrices here with random numbers
					e12 = alpha * h_d12[i2][i1]; //fill D matrix with random numbers
					e22 = alpha * h_d22[i2][i1];
					r00 = h_r[i2  ][i1  ];
					r01 = h_r[i2  ][i1-1];
					r10 = h_r[i2-1][i1  ];
					r11 = h_r[i2-1][i1-1];
					rs = 0.25f*(r00+r01+r10+r11);
					ra = r00-r11;
					rb = r01-r10;
					r1 = ra-rb;
					r2 = ra+rb;
					s1 = e11*r1+e12*r2;
					s2 = e12*r1+e22*r2;
					sa = s1+s2;
					sb = s1-s2;
					s_p[i2  ][i1  ] += sa+rs;
					s_p[i2  ][i1-1] -= sb-rs;
					s_p[i2-1][i1  ] += sb+rs;
					s_p[i2-1][i1-1] -= sa-rs;

				}
			}

		
			//SOS OpenCL Image Creation


			cl_mem d_s, d_r, d_d11, d_d12, d_d22;


			//Allocating memory on the GPU

			d_r = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				std::cout << "OpenCL Error code: " << err << std::endl;
				system("pause");
				exit(1);
			}

			d_d11 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				std::cout << "OpenCL Error code: " << err << std::endl;
				system("pause");
				exit(1);
			}

			d_d12 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				std::cout << "OpenCL Error code: " << err << std::endl;
				system("pause");
				exit(1);
			}

			d_d22 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				std::cout << "OpenCL Error code: " << err << std::endl;
				system("pause");
				exit(1);
			}

			d_s = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*(n1+1)*(n2+1), NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				std::cout << "OpenCL Error code: " << err << std::endl;
				system("pause");
				exit(1);
			}


			//Copying the 5 different packed arrays to the GPU

			err = clEnqueueWriteBuffer(queue, d_r, CL_FALSE, 0, sizeof(float)*dims, h_r1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not write to buffer." << std::endl;
				std::cout << "OpenCL Error Code: " << std::endl;
				system("pause");
				exit(1);
			}
	
			err = clEnqueueWriteBuffer(queue, d_d11, CL_FALSE, 0, sizeof(float)*dims, h_d11_1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not write to buffer." << std::endl;
				std::cout << "OpenCL Error Code: " << std::endl;
				system("pause");
				exit(1);
			}

			err = clEnqueueWriteBuffer(queue, d_d12, CL_FALSE, 0, sizeof(float)*dims, h_d12_1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not write to buffer." << std::endl;
				std::cout << "OpenCL Error Code: " << std::endl;
				system("pause");
				exit(1);
			}
	
			err = clEnqueueWriteBuffer(queue, d_d22, CL_FALSE, 0, sizeof(float)*dims, h_d22_1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not write to buffer." << std::endl;
				std::cout << "OpenCL Error Code: " << std::endl;
				system("pause");
				exit(1);
			}

			//Calling the setMem kernel to zero out the input s array on the GPU

			err = clSetKernelArg(kernel0, 0, sizeof(cl_mem), &d_s);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

	
			err = clSetKernelArg(kernel0, 1, sizeof(int), &value);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}


			err = clSetKernelArg(kernel0, 2, sizeof(int), &dims);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			size_t oned_global_work_size = dims;
			err = clEnqueueNDRangeKernel(queue, kernel0, 1, NULL, &oned_global_work_size, NULL, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not execute the kernel." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				system("pause");
				exit(1);
			}


			size_t local_group_size[2] = {32,32};
			size_t mapped_n1 = n2 / 2 ;
			size_t mapped_n2 = n1 / 2 ;
			size_t global_group_size_block[2] = {ceil((mapped_n1/local_group_size[0]) + 1) * local_group_size[0], 
                                         ceil((mapped_n2/local_group_size[1]) + 1) * local_group_size[1]}; //this needs to be changed for both the x and the y dimensions
			size_t global_group_size_norm[2]  = {1, 1};

			err = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_r);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			err = clSetKernelArg(kernel1, 1, sizeof(cl_mem), &d_d11);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}


			err = clSetKernelArg(kernel1, 2, sizeof(cl_mem), &d_d12);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			err = clSetKernelArg(kernel1, 3, sizeof(cl_mem), &d_d22);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			err = clSetKernelArg(kernel1, 4, sizeof(cl_mem), &d_s);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			err = clSetKernelArg(kernel1, 5, sizeof(float), &alpha);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			err = clSetKernelArg(kernel1, 6, sizeof(int), &n1);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

	
			err = clSetKernelArg(kernel1, 7, sizeof(int), &n2);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}
		

			for (int offsetx = 0; offsetx < 2; ++offsetx) {
				for (int offsety = 0; offsety < 2; ++offsety) {
					err = clSetKernelArg(kernel1, 8, sizeof(int), &offsetx);
					if(err != CL_SUCCESS){
        				std::cout << "Error: Could not set the kernel argument." << std::endl;
						std::cout << "OpenCL error code: " << err << std::endl;
						exit(1);
					}
					err = clSetKernelArg(kernel1, 9, sizeof(int), &offsety);
					if(err != CL_SUCCESS){
        				std::cout << "Error: Could not set the kernel argument." << std::endl;
						std::cout << "OpenCL error code: " << err << std::endl;
						exit(1);
					}
    
					err = clEnqueueNDRangeKernel(queue, kernel1, 2, NULL, global_group_size_block, local_group_size, 0, NULL, NULL);
					if(err != CL_SUCCESS){
        				std::cout << "Error: Could not execute the kernel." << std::endl;
						std::cout << "OpenCL error code: " << err << std::endl;
						system("pause");
						exit(1);
				
					}
			
				}
		
			}

			err = clEnqueueReadBuffer(queue, d_s, CL_TRUE, 0, sizeof(float)*(n1+1)*(n2+1), h_s1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not read data from the device." << std::endl;
				std::cout << "OpenCL error code: " << std::endl;
				exit(1);
			}

			//Freeing up GPU memory

			clReleaseMemObject(d_r);
			clReleaseMemObject(d_s);
			clReleaseMemObject(d_d11);
			clReleaseMemObject(d_d12);
			clReleaseMemObject(d_d22);
			clReleaseKernel(kernel0);
			clReleaseKernel(kernel1);
			clReleaseCommandQueue(queue);
			clReleaseProgram(program);
			clReleaseContext(context);

			//unpacking the array

			k = 0;
			for (int i = 0; i < n1; i++){
				for (int j = 0; j < n2; j++){
					h_s[i][j] = h_s1[k];
					k++;
		
				}
	
			}


			//verify the results

			bool check = true;
			int numfails = 0;
			for(int i = 0; i < n1; i++)
			{
				for(int j = 0; j < n2; j++)
				{
					if(fabs(s[i][j] - h_s[i][j]) < 0.001f) //changed here
					{
						continue;
					}
					else
					{
						check = false;
						printf("Check failed at array element (%d, %d)\n", i, j);
						printf("CPU Value: %f\n", s[i][j]);
						printf("GPU Value: %f\n", h_s[i][j]);
						++numfails;
					}
					if(true || fabs(s[i][j] - s_p[i][j]) < 0.001f)
					{
						continue;
					}
					else
					{
						check = false;
						printf("Check failed at array element (%d, %d)\n", i, j);
						printf("CPU Value: %f\n", s[i][j]);
						printf("CPU2 Value: %f\n", s_p[i][j]);
						printf("Percent Difference: %.5f Percent\n", s[i][j]/h_s[i][j]*100.f);
					}
				}
			}

			if(check == true)
				std::cout << "Check Passed!" << std::endl << std::endl;
			else
				std::cout << "Check Failed! Number of fails:" << numfails << std::endl << std::endl;


				
			for(int i = 0; i < n1; ++i)
			{
				for(int j = 0; j < n2; ++j)
				{
					inputData << h_r[i][j] << "\t";
				}
				inputData << std::endl;
			}

			for(int i = 0; i < n1; ++i)
			{
				for(int j = 0; j < n2; ++j)
				{
					cpuOutput << s[i][j] << " ";
					gpuOutput << h_s[i][j] << " ";
				}
				cpuOutput << std::endl;
				gpuOutput << std::endl;
			}

	
			cpuOutput.close();
			gpuOutput.close();
			inputData.close();

					//Deallocating the 1D arrays

			delete [] h_r1;
			delete [] h_s1;
			delete [] h_d11_1;
			delete [] h_d12_1;
			delete [] h_d22_1;
	
			//Deallocating the 2D arrays
			for (int i = 0; i < n1; i++)
			{
				delete [] h_r[i];
				delete [] h_s[i];
				delete [] s_p[i];
				delete [] h_d11[i];
				delete [] h_d12[i];
				delete [] h_d22[i];
			}
            
			delete [] h_r;
			delete [] h_s;
			delete [] s_p;
			delete [] h_d11;
			delete [] h_d12;
			delete [] h_d22;


			system("python Display_Smoothed_Image.py");

		}

#pragma endregion

#pragma region SOS Benchmark

		else if(programType == 2)
		{

			int maxTime = 5;
			int cpucount = 0;
			sw.restart();
			while(sw.getTime() < maxTime){
			//here is the original smoothing algorithm. This is what I need programmed in OpenCL
				for(int i2 = 1; i2 < n1; ++i2)
				{
					for(int i1 = 1; i1 < n2; ++i1)
					{

						e11 = alpha * h_d11[i2][i1]; //fill matrices here with random numbers
						e12 = alpha * h_d12[i2][i1]; //fill D matrix with random numbers
						e22 = alpha * h_d22[i2][i1];
						r00 = h_r[i2  ][i1  ];
						r01 = h_r[i2  ][i1-1];
						r10 = h_r[i2-1][i1  ];
						r11 = h_r[i2-1][i1-1];
						rs = 0.25f*(r00+r01+r10+r11);
						ra = r00-r11;
						rb = r01-r10;
						r1 = ra-rb;
						r2 = ra+rb;
						s1 = e11*r1+e12*r2;
						s2 = e12*r1+e22*r2;
						sa = s1+s2;
						sb = s1-s2;
						s[i2  ][i1  ] += sa+rs;
						s[i2  ][i1-1] -= sb-rs;
						s[i2-1][i1  ] += sb+rs;
						s[i2-1][i1-1] -= sa-rs;

					}
				}


				cpucount++;
			}

			sw.stop();
			float cputime = sw.getTime();

			cl_mem d_s, d_r, d_d11, d_d12, d_d22;


			//Allocating memory on the GPU

			d_r = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				std::cout << "OpenCL Error code: " << err << std::endl;
				system("pause");
				exit(1);
			}

			d_d11 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				std::cout << "OpenCL Error code: " << err << std::endl;
				system("pause");
				exit(1);
			}

			d_d12 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				std::cout << "OpenCL Error code: " << err << std::endl;
				system("pause");
				exit(1);
			}

			d_d22 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				std::cout << "OpenCL Error code: " << err << std::endl;
				system("pause");
				exit(1);
			}

			d_s = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*(n1+1)*(n2+1), NULL, &err);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not create the buffer." << std::endl;
				std::cout << "OpenCL Error code: " << err << std::endl;
				system("pause");
				exit(1);
			}


			//Copying the 5 different packed arrays to the GPU

			err = clEnqueueWriteBuffer(queue, d_r, CL_FALSE, 0, sizeof(float)*dims, h_r1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not write to buffer." << std::endl;
				std::cout << "OpenCL Error Code: " << std::endl;
				system("pause");
				exit(1);
			}
	
			err = clEnqueueWriteBuffer(queue, d_d11, CL_FALSE, 0, sizeof(float)*dims, h_d11_1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not write to buffer." << std::endl;
				std::cout << "OpenCL Error Code: " << std::endl;
				system("pause");
				exit(1);
			}

			err = clEnqueueWriteBuffer(queue, d_d12, CL_FALSE, 0, sizeof(float)*dims, h_d12_1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not write to buffer." << std::endl;
				std::cout << "OpenCL Error Code: " << std::endl;
				system("pause");
				exit(1);
			}
	
			err = clEnqueueWriteBuffer(queue, d_d22, CL_FALSE, 0, sizeof(float)*dims, h_d22_1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not write to buffer." << std::endl;
				std::cout << "OpenCL Error Code: " << std::endl;
				system("pause");
				exit(1);
			}

			//Calling the setMem kernel to zero out the input s array on the GPU

			err = clSetKernelArg(kernel0, 0, sizeof(cl_mem), &d_s);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

	
			err = clSetKernelArg(kernel0, 1, sizeof(int), &value);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}


			err = clSetKernelArg(kernel0, 2, sizeof(int), &dims);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			size_t oned_global_work_size = dims;
			err = clEnqueueNDRangeKernel(queue, kernel0, 1, NULL, &oned_global_work_size, NULL, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not execute the kernel." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				system("pause");
				exit(1);
			}


			size_t local_group_size[2] = {32,32};
			size_t mapped_n1 = n2 / 2 ;
			size_t mapped_n2 = n1 / 2 ;
			size_t global_group_size_block[2] = {ceil((mapped_n1/local_group_size[0]) + 1) * local_group_size[0], 
                                         ceil((mapped_n2/local_group_size[1]) + 1) * local_group_size[1]}; //this needs to be changed for both the x and the y dimensions
			size_t global_group_size_norm[2]  = {1, 1};

			err = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_r);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			err = clSetKernelArg(kernel1, 1, sizeof(cl_mem), &d_d11);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}


			err = clSetKernelArg(kernel1, 2, sizeof(cl_mem), &d_d12);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			err = clSetKernelArg(kernel1, 3, sizeof(cl_mem), &d_d22);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			err = clSetKernelArg(kernel1, 4, sizeof(cl_mem), &d_s);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			err = clSetKernelArg(kernel1, 5, sizeof(float), &alpha);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

			err = clSetKernelArg(kernel1, 6, sizeof(int), &n1);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}

	
			err = clSetKernelArg(kernel1, 7, sizeof(int), &n2);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not set the kernel argument." << std::endl;
				std::cout << "OpenCL error code: " << err << std::endl;
				exit(1);
			}
		
			int gpucount = 0;
			sw.restart();
			while(sw.getTime() < maxTime){

				for (int offsetx = 0; offsetx < 2; ++offsetx) {
					for (int offsety = 0; offsety < 2; ++offsety) {
						err = clSetKernelArg(kernel1, 8, sizeof(int), &offsetx);
						if(err != CL_SUCCESS){
        					std::cout << "Error: Could not set the kernel argument." << std::endl;
							std::cout << "OpenCL error code: " << err << std::endl;
							exit(1);
						}
						err = clSetKernelArg(kernel1, 9, sizeof(int), &offsety);
						if(err != CL_SUCCESS){
        					std::cout << "Error: Could not set the kernel argument." << std::endl;
							std::cout << "OpenCL error code: " << err << std::endl;
							exit(1);
						}
    
						err = clEnqueueNDRangeKernel(queue, kernel1, 2, NULL, global_group_size_block, local_group_size, 0, NULL, NULL);
						if(err != CL_SUCCESS){
        					std::cout << "Error: Could not execute the kernel." << std::endl;
							std::cout << "OpenCL error code: " << err << std::endl;
							system("pause");
							exit(1);
				
						}
			
					}
		
				}
			
				gpucount++;
			
			}

			sw.stop();
			float gputime = sw.getTime();

			err = clEnqueueReadBuffer(queue, d_s, CL_TRUE, 0, sizeof(float)*(n1+1)*(n2+1), h_s1, 0, NULL, NULL);
			if(err != CL_SUCCESS){
				std::cout << "Error: Could not read data from the device." << std::endl;
				std::cout << "OpenCL error code: " << std::endl;
				exit(1);
			}

			//Freeing up GPU memory

			clReleaseMemObject(d_r);
			clReleaseMemObject(d_s);
			clReleaseMemObject(d_d11);
			clReleaseMemObject(d_d12);
			clReleaseMemObject(d_d22);
			clReleaseKernel(kernel0);
			clReleaseKernel(kernel1);
			clReleaseCommandQueue(queue);
			clReleaseProgram(program);
			clReleaseContext(context);

			//unpacking the array

			k = 0;
			for (int i = 0; i < n1; i++){
				for (int j = 0; j < n2; j++){
					h_s[i][j] = h_s1[k];
					k++;
		
				}
	
			}

			float cpucountf = float(cpucount);
			float gpucountf = float(gpucount);
			float numflops = 29.0f;

			float cpumflops = (cpucountf * numflops * (n1-1) * (n2-1) * 1.0e-6)/cputime; //I dont believe this is correct and I need to be more rigorous with this. Determine this with a very small matrix 2x2 first.
			float gpumflops = (gpucountf * numflops * (n1-1) * (n2-1) * 1.0e-6)/gputime;

			std::cout << "CPU Benchmark: " << cpumflops << " MFLOPS" << std::endl;
			std::cout << "GPU Benchmark: " << gpumflops << " MFLOPS" << std::endl;

			//Deallocating the 1D arrays

			delete [] h_r1;
			delete [] h_s1;
			delete [] h_d11_1;
			delete [] h_d12_1;
			delete [] h_d22_1;
	
			//Deallocating the 2D arrays
			for (int i = 0; i < n1; i++)
			{
				delete [] h_r[i];
				delete [] h_s[i];
				delete [] s_p[i];
				delete [] h_d11[i];
				delete [] h_d12[i];
				delete [] h_d22[i];
			}
            
			delete [] h_r;
			delete [] h_s;
			delete [] s_p;
			delete [] h_d11;
			delete [] h_d12;
			delete [] h_d22;



		}

#pragma endregion

		else if(programType == 3)
		{
			for(int size1 = 1; size1 < 2048; size1*=2)
			{
				std::cout << "Benchmark for array size: " << size1 << std::endl << std::endl;
				int size2 = size1 + 2;
				int newdims = size1 * size2;

				float **h_r   = new float*[size1];
				float **h_s   = new float*[size1];
				float **s     = new float*[size1];
				float **s_p    = new float*[size1];
				float **h_d11 = new float*[size1];
				float **h_d12 = new float*[size1];
				float **h_d22 = new float*[size1];

				for(int i = 0; i < size1; i++){
                
					h_r[i]   = new float [size2];
					h_s[i]   = new float [size2];
					s[i]     = new float [size2];
					s_p[i]   = new float [size2];
					h_d11[i] = new float [size2];
					h_d12[i] = new float [size2];
					h_d22[i] = new float [size2];

                
					//Initializing the arrays.
					for(int j = 0; j < size2; j++){

						h_r[i][j]   = (float)rand()/(float)RAND_MAX;
						h_d11[i][j] = (float)rand()/(float)RAND_MAX;
						h_d12[i][j] = (float)rand()/(float)RAND_MAX;
						h_d22[i][j] = (float)rand()/(float)RAND_MAX;
						h_s[i][j]   = 0.0f;
						s[i][j]     = 0.0f;
						s_p[i][j]   = 0.0f;
                    
					}
                
				}

					float *h_r1 = new float[newdims];
					float *h_s1 = new float[(size1+1)*(size2+1)];
					float *h_d11_1 = new float[newdims];
					float *h_d12_1 = new float[newdims];
					float *h_d22_1 = new float[newdims];
                
					//packing arrays
					int k = 0;
					for (int i = 0; i < size1; i++){
						for (int j = 0; j < size2; j++){
							h_r1[k]   = h_r[i][j];
							h_s1[k]   = h_s[i][j];
							h_d11_1[k] = h_d11[i][j];
							h_d12_1[k] = h_d12[i][j];
							h_d22_1[k] = h_d22[i][j];
							k++;
						}
                    
					}

				int maxTime = 5;
				int cpucount = 0;
				sw.restart();
				while(sw.getTime() < maxTime){
				//here is the original smoothing algorithm. This is what I need programmed in OpenCL
					for(int i2 = 1; i2 < size1; ++i2)
					{
						for(int i1 = 1; i1 < size2; ++i1)
						{

							e11 = alpha * h_d11[i2][i1]; //fill matrices here with random numbers
							e12 = alpha * h_d12[i2][i1]; //fill D matrix with random numbers
							e22 = alpha * h_d22[i2][i1];
							r00 = h_r[i2  ][i1  ];
							r01 = h_r[i2  ][i1-1];
							r10 = h_r[i2-1][i1  ];
							r11 = h_r[i2-1][i1-1];
							rs = 0.25f*(r00+r01+r10+r11);
							ra = r00-r11;
							rb = r01-r10;
							r1 = ra-rb;
							r2 = ra+rb;
							s1 = e11*r1+e12*r2;
							s2 = e12*r1+e22*r2;
							sa = s1+s2;
							sb = s1-s2;
							s[i2  ][i1  ] += sa+rs;
							s[i2  ][i1-1] -= sb-rs;
							s[i2-1][i1  ] += sb+rs;
							s[i2-1][i1-1] -= sa-rs;

						}
					}


					cpucount++;
				}

				sw.stop();
				float cputime = sw.getTime();

				context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not create a context." << std::endl;
					system("pause");
					exit(1);
				}

				fileName = "ref_sosmoothing.cl";
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
                
				err = clBuildProgram(program, 1, &device, "-cl-strict-aliasing -cl-unsafe-math-optimizations -cl-finite-math-only -cl-fast-relaxed-math -DMAC", NULL, NULL); //here I use the cl-opt-disable flag to diable the FMA rounding. This was the only way I could get both CPU and GPU output to match
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
           
				kernel0 = clCreateKernel(program, "setMem", &err);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not create the kernel." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					system("pause");
					exit(1);
				}

				kernel1 = clCreateKernel(program, "soSmoothingNew", &err);  //first kernel I wrote. It made sense to me to write it this way but it is hard to read
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not create the kernel." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					system("pause");
					exit(1);
				}
                
				queue = clCreateCommandQueue(context, device, 0, &err);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not create the queue." << std::endl;
					system("pause");
					exit(1);
				}

				cl_mem d_s, d_r, d_d11, d_d12, d_d22;


				//Allocating memory on the GPU

				d_r = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*newdims, NULL, &err);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not create the buffer." << std::endl;
					std::cout << "OpenCL Error code: " << err << std::endl;
					system("pause");
					exit(1);
				}

				d_d11 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*newdims, NULL, &err);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not create the buffer." << std::endl;
					std::cout << "OpenCL Error code: " << err << std::endl;
					system("pause");
					exit(1);
				}

				d_d12 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*newdims, NULL, &err);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not create the buffer." << std::endl;
					std::cout << "OpenCL Error code: " << err << std::endl;
					system("pause");
					exit(1);
				}

				d_d22 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*newdims, NULL, &err);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not create the buffer." << std::endl;
					std::cout << "OpenCL Error code: " << err << std::endl;
					system("pause");
					exit(1);
				}

				d_s = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*(size1+1)*(size2+1), NULL, &err);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not create the buffer." << std::endl;
					std::cout << "OpenCL Error code: " << err << std::endl;
					system("pause");
					exit(1);
				}


				//Copying the 5 different packed arrays to the GPU

				err = clEnqueueWriteBuffer(queue, d_r, CL_FALSE, 0, sizeof(float)*newdims, h_r1, 0, NULL, NULL);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not write to buffer." << std::endl;
					std::cout << "OpenCL Error Code: " << std::endl;
					system("pause");
					exit(1);
				}
	
				err = clEnqueueWriteBuffer(queue, d_d11, CL_FALSE, 0, sizeof(float)*newdims, h_d11_1, 0, NULL, NULL);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not write to buffer." << std::endl;
					std::cout << "OpenCL Error Code: " << std::endl;
					system("pause");
					exit(1);
				}

				err = clEnqueueWriteBuffer(queue, d_d12, CL_FALSE, 0, sizeof(float)*newdims, h_d12_1, 0, NULL, NULL);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not write to buffer." << std::endl;
					std::cout << "OpenCL Error Code: " << std::endl;
					system("pause");
					exit(1);
				}
	
				err = clEnqueueWriteBuffer(queue, d_d22, CL_FALSE, 0, sizeof(float)*newdims, h_d22_1, 0, NULL, NULL);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not write to buffer." << std::endl;
					std::cout << "OpenCL Error Code: " << std::endl;
					system("pause");
					exit(1);
				}

				//Calling the setMem kernel to zero out the input s array on the GPU

				err = clSetKernelArg(kernel0, 0, sizeof(cl_mem), &d_s);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not set the kernel argument." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					exit(1);
				}

	
				err = clSetKernelArg(kernel0, 1, sizeof(int), &value);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not set the kernel argument." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					exit(1);
				}


				err = clSetKernelArg(kernel0, 2, sizeof(int), &newdims);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not set the kernel argument." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					exit(1);
				}

				size_t oned_global_work_size = newdims;
				err = clEnqueueNDRangeKernel(queue, kernel0, 1, NULL, &oned_global_work_size, NULL, 0, NULL, NULL);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not execute the kernel." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					system("pause");
					exit(1);
				}


				size_t local_group_size[2] = {32,32};
				size_t mapped_n1 = n2 / 2 ;
				size_t mapped_n2 = n1 / 2 ;
				size_t global_group_size_block[2] = {ceil((mapped_n1/local_group_size[0]) + 1) * local_group_size[0], 
											 ceil((mapped_n2/local_group_size[1]) + 1) * local_group_size[1]}; //this needs to be changed for both the x and the y dimensions
				size_t global_group_size_norm[2]  = {1, 1};

				err = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_r);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not set the kernel argument." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					exit(1);
				}

				err = clSetKernelArg(kernel1, 1, sizeof(cl_mem), &d_d11);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not set the kernel argument." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					exit(1);
				}


				err = clSetKernelArg(kernel1, 2, sizeof(cl_mem), &d_d12);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not set the kernel argument." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					exit(1);
				}

				err = clSetKernelArg(kernel1, 3, sizeof(cl_mem), &d_d22);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not set the kernel argument." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					exit(1);
				}

				err = clSetKernelArg(kernel1, 4, sizeof(cl_mem), &d_s);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not set the kernel argument." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					exit(1);
				}

				err = clSetKernelArg(kernel1, 5, sizeof(float), &alpha);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not set the kernel argument." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					exit(1);
				}

				err = clSetKernelArg(kernel1, 6, sizeof(int), &size1);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not set the kernel argument." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					exit(1);
				}

	
				err = clSetKernelArg(kernel1, 7, sizeof(int), &size2);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not set the kernel argument." << std::endl;
					std::cout << "OpenCL error code: " << err << std::endl;
					exit(1);
				}
		
				int gpucount = 0;
				sw.restart();
				while(sw.getTime() < maxTime){

					for (int offsetx = 0; offsetx < 2; ++offsetx) {
						for (int offsety = 0; offsety < 2; ++offsety) {
							err = clSetKernelArg(kernel1, 8, sizeof(int), &offsetx);
							if(err != CL_SUCCESS){
        						std::cout << "Error: Could not set the kernel argument." << std::endl;
								std::cout << "OpenCL error code: " << err << std::endl;
								exit(1);
							}
							err = clSetKernelArg(kernel1, 9, sizeof(int), &offsety);
							if(err != CL_SUCCESS){
        						std::cout << "Error: Could not set the kernel argument." << std::endl;
								std::cout << "OpenCL error code: " << err << std::endl;
								exit(1);
							}
    
							err = clEnqueueNDRangeKernel(queue, kernel1, 2, NULL, global_group_size_block, local_group_size, 0, NULL, NULL);
							if(err != CL_SUCCESS){
        						std::cout << "Error: Could not execute the kernel." << std::endl;
								std::cout << "OpenCL error code: " << err << std::endl;
								system("pause");
								exit(1);
				
							}
			
						}
		
					}
			
					gpucount++;
			
				}

				sw.stop();
				float gputime = sw.getTime();

				err = clEnqueueReadBuffer(queue, d_s, CL_TRUE, 0, sizeof(float)*(size1+1)*(size2+1), h_s1, 0, NULL, NULL);
				if(err != CL_SUCCESS){
					std::cout << "Error: Could not read data from the device." << std::endl;
					std::cout << "OpenCL error code: " << std::endl;
					exit(1);
				}

				//Freeing up GPU memory

				clReleaseMemObject(d_r);
				clReleaseMemObject(d_s);
				clReleaseMemObject(d_d11);
				clReleaseMemObject(d_d12);
				clReleaseMemObject(d_d22);
				clReleaseKernel(kernel0);
				clReleaseKernel(kernel1);
				clReleaseCommandQueue(queue);
				clReleaseProgram(program);
				clReleaseContext(context);

				//unpacking the array

				k = 0;
				for (int i = 0; i < size1; i++){
					for (int j = 0; j < size2; j++){
						h_s[i][j] = h_s1[k];
						k++;
		
					}
	
				}

				float cpucountf = float(cpucount);
				float gpucountf = float(gpucount);
				float numflops = 29.0f;

				float cpumflops = (cpucountf * numflops * (size1-1) * (size2-1) * 1.0e-6)/cputime; //I dont believe this is correct and I need to be more rigorous with this. Determine this with a very small matrix 2x2 first.
				float gpumflops = (gpucountf * numflops * (size1-1) * (size2-1) * 1.0e-6)/gputime;

				std::cout << "CPU Benchmark: " << cpumflops << " MFLOPS" << std::endl;
				std::cout << "GPU Benchmark: " << gpumflops << " MFLOPS" << std::endl << std::endl;

				//write data to file

				perfData << cpumflops << " " << gpumflops << std::endl;


						//Deallocating the 1D arrays

				delete [] h_r1;
				delete [] h_s1;
				delete [] h_d11_1;
				delete [] h_d12_1;
				delete [] h_d22_1;
	
				//Deallocating the 2D arrays
				for (int i = 0; i < size1; i++)
				{
					delete [] h_r[i];
					delete [] h_s[i];
					delete [] s_p[i];
					delete [] h_d11[i];
					delete [] h_d12[i];
					delete [] h_d22[i];
				}
            
				delete [] h_r;
				delete [] h_s;
				delete [] s_p;
				delete [] h_d11;
				delete [] h_d12;
				delete [] h_d22;

			}

			system("python sos_perf_plot.py");

		}


#pragma region Free up Memory

	system("pause");
	return 0;
}