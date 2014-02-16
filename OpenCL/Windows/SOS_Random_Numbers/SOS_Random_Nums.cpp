
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

	const int n1 = 1024;
	const int n2 = n1+1;
	const int dims = n1*n2;
	int value = 0;

	cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel1, kernel2, kernel3;
	cl_uint num_platforms;


	//A cl_int used to store error flags that are returned if OpenCL function does not execute properly
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
	std::ofstream dTensors;
	cpuOutput.open("..\\TwoD_Images\\cpuoutput.txt");
	gpuOutput.open("..\\TwoD_Images\\gpuoutput.txt");
	inputData.open("..\\TwoD_Images\\inputData.txt");
	dTensors.open("..\\TwoD_Images\\dTensorData.txt");

	Stopwatch sw;

	float e11, e12, e22;
	float r00, r01, r10, r11;
	float rs, ra, rb, sa, sb;
	float r1, r2, s1, s2;
	float alpha = 18.0f;
	float d11sc = 5;
	float d12sc = 10;
	float d22sc = 15;
	

#pragma region OpenCL Device/Platform/Queue setup, etc.

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

	fileName = "sosmoothing.cl";
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
                
	err = clBuildProgram(program, 1, &device, "-cl-opt-disable", NULL, NULL);
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
           
	kernel1 = clCreateKernel(program, "setMem", &err);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not create the kernel." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		system("pause");
		exit(1);
	}

	kernel2 = clCreateKernel(program, "soSmoothing", &err);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not create the kernel." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		system("pause");
		exit(1);
	}

	kernel3 = clCreateKernel(program, "soSmoothingNew", &err);
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

#pragma region Initializing and Packing the arrays

	float **h_r   = new float*[n1];
	float **h_s   = new float*[n1];
	float **s     = new float*[n1];
	float **s_1   = new float*[n1];
	float **h_d11 = new float*[n1];
	float **h_d12 = new float*[n1];
	float **h_d22 = new float*[n1];

	for(int i = 0; i < n1; i++){
                
		h_r[i]   = new float [n2];
		h_s[i]   = new float [n2];
		s[i]     = new float [n2];
		s_1[i]   = new float [n2];
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
			s_1[i][j]   = 0.0f;
                    
		}
                
	}

		float *h_r1 = new float[dims];
		float *h_s1 = new float[dims];
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

#pragma region Structure Oriented Smoothing CPU


	int maxTime = 5;
	int cpucount = 0;
	//sw.restart();
	//while(sw.getTime() < maxTime){


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
				//s[i2  ][i1  ] += e12+e11;
				//s[i2  ][i1-1] -= rs;
				//s[i2-1][i1  ] += rs;
				//s[i2-1][i1-1] -= rs;

			}
		}


		cpucount++;
	//}

	//sw.stop();
	float cputime = sw.getTime();

#pragma endregion



	//This bit of code is the above main algorithm, but put into only four equations. I did this to try and get the code for the OpenCL kernel
#pragma region Structure Oriented Smoothing CPU four Eqns


	for(int i2 = 1; i2 < n1; ++i2)
	{
		for(int i1 = 1; i1 < n2; ++i1)
		{
			s_1[i2  ][i1  ] += alpha * h_d11_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])-(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))+alpha * h_d12_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])+(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))+(alpha * h_d12_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])-(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))+alpha * h_d22_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])+(h_r[i2  ][i1-1]-h_r[i2-1][i1  ])))+0.25f*(h_r[i2  ][i1  ]+h_r[i2  ][i1-1]+h_r[i2-1][i1  ]+h_r[i2-1][i1-1]);
			s_1[i2  ][i1-1] -= alpha * h_d11_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])-(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))+alpha * h_d12_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])+(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))-(alpha * h_d12_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])-(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))+alpha * h_d22_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])+(h_r[i2  ][i1-1]-h_r[i2-1][i1  ])))-0.25f*(h_r[i2  ][i1  ]+h_r[i2  ][i1-1]+h_r[i2-1][i1  ]+h_r[i2-1][i1-1]);
			s_1[i2-1][i1  ] += alpha * h_d11_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])-(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))+alpha * h_d12_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])+(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))-(alpha * h_d12_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])-(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))+alpha * h_d22_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])+(h_r[i2  ][i1-1]-h_r[i2-1][i1  ])))+0.25f*(h_r[i2  ][i1  ]+h_r[i2  ][i1-1]+h_r[i2-1][i1  ]+h_r[i2-1][i1-1]);
			s_1[i2-1][i1-1] -= alpha * h_d11_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])-(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))+alpha * h_d12_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])+(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))+(alpha * h_d12_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])-(h_r[i2  ][i1-1]-h_r[i2-1][i1  ]))+alpha * h_d22_1[i2*n2 + i1]*((h_r[i2  ][i1  ]-h_r[i2-1][i1-1])+(h_r[i2  ][i1-1]-h_r[i2-1][i1  ])))-0.25f*(h_r[i2  ][i1  ]+h_r[i2  ][i1-1]+h_r[i2-1][i1  ]+h_r[i2-1][i1-1]);
		
		}
	}


#pragma endregion

	//This is the same as the code from above but it uses 1D arrays
#pragma region Structure Oriented Smoothing CPU One D four Eqns

	for(int i2 = 1; i2 < n1; ++i2)
	{
		for(int i1 = 1; i1 < n2; ++i1)
		{
			h_s1[i2*n2     +     i1] += alpha * h_d11_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])-(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))+alpha * h_d12_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])+(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))+(alpha * h_d12_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])-(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))+alpha * h_d22_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])+(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1])))+0.25f*(h_r1[i2*n2 + i1]+h_r1[i2*n2 + (i1-1)]+h_r1[(i2-1)*n2 + i1]+h_r1[(i2-1)*n2 + (i1-1)]);
			h_s1[i2*n2     + (i1-1)] -= alpha * h_d11_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])-(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))+alpha * h_d12_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])+(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))-(alpha * h_d12_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])-(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))+alpha * h_d22_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])+(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1])))-0.25f*(h_r1[i2*n2 + i1]+h_r1[i2*n2 + (i1-1)]+h_r1[(i2-1)*n2 + i1]+h_r1[(i2-1)*n2 + (i1-1)]);
			h_s1[(i2-1)*n2 +     i1] += alpha * h_d11_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])-(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))+alpha * h_d12_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])+(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))-(alpha * h_d12_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])-(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))+alpha * h_d22_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])+(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1])))+0.25f*(h_r1[i2*n2 + i1]+h_r1[i2*n2 + (i1-1)]+h_r1[(i2-1)*n2 + i1]+h_r1[(i2-1)*n2 + (i1-1)]);
			h_s1[(i2-1)*n2 + (i1-1)] -= alpha * h_d11_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])-(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))+alpha * h_d12_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])+(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))+(alpha * h_d12_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])-(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1]))+alpha * h_d22_1[i2*n2 + i1]*((h_r1[i2*n2 + i1]-h_r1[(i2-1)*n2 + (i1-1)])+(h_r1[i2*n2 + (i1-1)]-h_r1[(i2-1)*n2 + i1])))-0.25f*(h_r1[i2*n2 + i1]+h_r1[i2*n2 + (i1-1)]+h_r1[(i2-1)*n2 + i1]+h_r1[(i2-1)*n2 + (i1-1)]);
		
		}
	}


#pragma endregion

	//The OpenCL code for this same algorithm
#pragma region Structure Oriented Smoothing OpenCL

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

	d_s = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*dims, NULL, &err);
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

#pragma region Using the setMem kernel to set all values to zero within s

	err = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_s);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	
	err = clSetKernelArg(kernel1, 1, sizeof(int), &value);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel1, 2, sizeof(int), &dims);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	size_t oned_global_work_size = dims;
	err = clEnqueueNDRangeKernel(queue, kernel1, 1, NULL, &oned_global_work_size, NULL, 0, NULL, NULL);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not execute the kernel." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		system("pause");
		exit(1);
	}

#pragma endregion

#pragma region The main kernel I am playing with

	err = clSetKernelArg(kernel2, 0, sizeof(cl_mem), &d_r);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel2, 1, sizeof(cl_mem), &d_d11);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}


	err = clSetKernelArg(kernel2, 2, sizeof(cl_mem), &d_d12);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel2, 3, sizeof(cl_mem), &d_d22);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel2, 4, sizeof(cl_mem), &d_s);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel2, 5, sizeof(float), &alpha);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel2, 6, sizeof(int), &n1);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	
	err = clSetKernelArg(kernel2, 7, sizeof(int), &n2);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	//Defining the local and global work group sizes and the launching the kernel on the GPU

	int gpucount = 0;
	//sw.restart();
	//while(sw.getTime() < maxTime){
		size_t local_group_size[3] = {32,32, 1};
		size_t global_group_size_block[3] = {ceil((n1/local_group_size[0]) + 1) * local_group_size[0], ceil((n2/local_group_size[1]) + 1) * local_group_size[1], 1}; //this needs to be changed for both the x and the y dimensions
		size_t global_group_size_norm[2]  = {1, 1};
		err = clEnqueueNDRangeKernel(queue, kernel2, 3, NULL, global_group_size_block, local_group_size, 0, NULL, NULL);
		if(err != CL_SUCCESS){
			std::cout << "Error: Could not execute the kernel." << std::endl;
			std::cout << "OpenCL error code: " << err << std::endl;
			system("pause");
			exit(1);
		}

		//clFinish(queue);

		gpucount++;
	//}
	//sw.stop();
	float gputime = sw.getTime();

	//Copying the data back from the gpu

#pragma endregion

#pragma region A kernel that I started to test

	err = clSetKernelArg(kernel3, 0, sizeof(cl_mem), &d_r);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel3, 1, sizeof(cl_mem), &d_d11);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}


	err = clSetKernelArg(kernel3, 2, sizeof(cl_mem), &d_d12);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel3, 3, sizeof(cl_mem), &d_d22);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel3, 4, sizeof(cl_mem), &d_s);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel3, 5, sizeof(int), &alpha);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel3, 6, sizeof(int), &n1);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	
	err = clSetKernelArg(kernel3, 7, sizeof(int), &n2);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	//err = clEnqueueNDRangeKernel(queue, kernel3, 2, NULL, global_group_size_block, local_group_size, 0, NULL, NULL);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not execute the kernel." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		system("pause");
		exit(1);
	}

#pragma endregion


#pragma region Copying data Back from the device and Freeing memory

	err = clEnqueueReadBuffer(queue, d_s, CL_TRUE, 0, sizeof(float)*dims, h_s1, 0, NULL, NULL);
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
	clReleaseKernel(kernel1);
	clReleaseKernel(kernel2);
	clReleaseKernel(kernel3);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseContext(context);

#pragma endregion



#pragma endregion

#pragma region Unpack the array

	k = 0;
	for (int i = 0; i < n1; i++){
		for (int j = 0; j < n2; j++){
			h_s[i][j] = h_s1[k];
			k++;
		}
	}

#pragma endregion

#pragma region CheckSum

	int cpuSum1 = 0;
	int cpuSum2 = 0;
	int gpuSum = 0;
	bool check = true;
	for(int i = 0; i < n1; i++)
	{
		for(int j = 0; j < n2; j++)
		{
			if(s[i][j] == h_s[i][j])
			{
				continue;
			}
			else
			{
				check = false;
				printf("Check failed at array element (%d, %d)\n", i, j);
				printf("CPU Value: %f\n", s[i][j]);
				printf("GPU Value: %f\n", h_s[i][j]);
				//printf("Percent Difference: %.5f Percent\n", s[i][j]/h_s[i][j]*100.f);
				system("pause");
				//exit(1);
			}
		}
	}

	if(check == true)
		std::cout << "Check Passed!" << std::endl << std::endl;
	else
		std::cout << "Check Failed!" << std::endl << std::endl;


#pragma endregion


#pragma region Write s and h_s  and h_d* values to text files

	
	////inputData << "Input Array:" << std::endl << std::endl;
	
	for(int i = 0; i < n1; ++i)
	{
		for(int j = 0; j < n2; ++j)
		{
			inputData << h_r[i][j] << "\t";
		}
		inputData << std::endl;
	}
	
	////cpuOutput << "CPU Output Array:" << std::endl << std::endl;
	////gpuOutput << "GPU Output Array:"  << std::endl << std::endl;

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

#pragma endregion

#pragma region Display Results

	float countf = float(cpucount);
	float numflops = 27.0f;

	float cpumflops = countf * numflops * n1 * n2 * 1.0e-6/sw.getTime(); //I dont believe this is correct and I need to be more rigorous with this. Determine this with a very small matrix 2x2 first.

#pragma endregion

#pragma region Freeing up memory

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
		delete [] h_d11[i];
		delete [] h_d12[i];
		delete [] h_d22[i];
	}
            
	delete [] h_r;
	delete [] h_s;
	delete [] h_d11;
	delete [] h_d12;
	delete [] h_d22;

#pragma endregion

	return 0;

}