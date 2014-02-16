

#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <CL\cl.h>
#include <string>



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


int main(void)
{

	int n1 = 50;
	int n2 = n1 + 1;

	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
    cl_kernel kernel1;
	cl_kernel kernel2;

	cl_int err;
    const char* fileName;
    const char* kernelName;

	FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    std::string programLog;

	float **x = new float*[n1];
	float **h_xx = new float*[n1];
	float *h_x1  = new float[n1*n2];

	std::ofstream gpuIndices;
	std::ofstream cpuIndices;

	for(int i = 0; i < n1; i++)
	{
		x[i] = new float[n2];
		h_xx[i] = new float[n2];	
		for(int j = 0; j < n2; j++)
		{
			x[i][j] = 0;
		}
	}

	for(int i = 1; i < n1; i++)
	{
		for(int j = 1; j < n2; j++)
		{
			x[i  ][j  ] = i;
			x[i  ][j-1] = i;
			x[i-1][j  ] = i;
			x[i-1][j-1] = i;

		}
	}
	
	err = clGetPlatformIDs(1, &platform, NULL);
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

	fileName = "simple2Dindex.cl";
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
           
	kernel1 = clCreateKernel(program, "setMem", &err);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not create the kernel." << std::endl;
		system("pause");
		exit(1);
	}

	kernel2 = clCreateKernel(program, "indexing", &err);
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


	cl_mem d_xx;

	d_xx = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*n1*n2, NULL, &err);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not create the buffer." << std::endl;
		std::cout << "OpenCL Error code: " << err << std::endl;
		system("pause");
		exit(1);
	}
	

	err = clEnqueueWriteBuffer(queue, d_xx, CL_FALSE, 0, sizeof(float)*n1*n2, h_x1, 0, NULL, NULL);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not write to buffer." << std::endl;
		std::cout << "OpenCL Error Code: " << std::endl;
		system("pause");
		exit(1);
	}

	
	err = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_xx);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	int value = 0;
	err = clSetKernelArg(kernel1, 1, sizeof(int), &value);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}
	
	int dims = n1*n2;
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
	
	err = clSetKernelArg(kernel2, 0, sizeof(cl_mem), &d_xx);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel2, 1, sizeof(int), &n1);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	err = clSetKernelArg(kernel2, 2, sizeof(int), &n2);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not set the kernel argument." << std::endl;
		std::cout << "OpenCL error code: " << err << std::endl;
		exit(1);
	}

	size_t local_work_group_size[2] = {32,32};
	size_t global_work_group_size[2] = {ceil((n1/local_work_group_size[0]) + 1) * local_work_group_size[0], ceil((n2/local_work_group_size[1]) + 1) * local_work_group_size[1]};
	size_t global_work_group_size_test[2] = {(n1 - 1 + local_work_group_size[0])/local_work_group_size[0], (n2 - 1 + local_work_group_size[1])/local_work_group_size[1]};
	size_t global_work_group_size_total[2] = {1,1};
	err = clEnqueueNDRangeKernel(queue, kernel2, 2, NULL, global_work_group_size_total, local_work_group_size, 0, NULL, NULL);
	if(err != CL_SUCCESS){
			std::cout << "Error: Could not execute the kernel." << std::endl;
			std::cout << "OpenCL error code: " << err << std::endl;
			system("pause");
			exit(1);
		}

	err = clEnqueueReadBuffer(queue, d_xx, CL_TRUE, 0, sizeof(float)*n1*n2, h_x1, 0, NULL, NULL);
	if(err != CL_SUCCESS){
		std::cout << "Error: Could not read data from the device." << std::endl;
		std::cout << "OpenCL error code: " << std::endl;
		exit(1);
	}


	clReleaseMemObject(d_xx);
	clReleaseKernel(kernel1);
	clReleaseKernel(kernel2);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);		
	clReleaseContext(context);

	int k = 0;
	for (int i = 0; i < n1; i++){
		for (int j = 0; j < n2; j++){
			h_xx[i][j] = h_x1[k];
			k++;
		}
	}



	bool check = true;
	for(int i = 0; i < n1; i++)
	{
		for(int j = 0; j < n2; j++)
		{
			if(x[i][j] == h_xx[i][j])
			{
				continue;
			}
			else
			{
				check = false;
				printf("Check failed at array element (%d, %d)\n", i, j);
				printf("CPU Value: %f\n", x[i][j]);
				printf("GPU Value: %f\n", h_xx[i][j]);
				//printf("Percent Difference: %.5f Percent\n", x[i][j]/h_xx[i][j]*100.f);
				system("pause");
				//exit(1);
			}
		}
	}

	if(check == true)
		std::cout << "Check Passed!" << std::endl << std::endl;
	else
	{
		std::cout << "Check Failed!" << std::endl << std::endl;
		system("pause");
	}


	cpuIndices.open("..\\cpuindices.txt");
	gpuIndices.open("..\\gpuindices.txt");

	for(int i = 0; i < n1; i++)
	{
		for(int j = 0; j < n2; j++)
		{
			cpuIndices << x[i][j];
			gpuIndices << h_xx[i][j];
		}
		cpuIndices << std::endl;
		gpuIndices << std::endl;
	}

	cpuIndices.close();
	gpuIndices.close();

	delete [] h_x1;

	
	//Deallocating the 2D arrays
	for (int i = 0; i < n1; i++)
	{
		delete [] x[i];
		delete [] h_xx[i];

	}
            
	delete [] x;
	delete [] h_xx;

	return 0;

}