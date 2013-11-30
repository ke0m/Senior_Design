

#include <iostream>

#include <vector>

#include <stdlib.h>

#include "GPU.h"

#include "ArraySumUtil.h"

int main(){

	GPU gpu("floptmem.cl", "arraysum", false);
	ArraySumUtil arr;


	int n1 = 2;
	int n2 = 2;
	int dims = n1*n2;
	int iters = 2;

	std::vector<std::vector<float> > h_xx(n1, std::vector<float>(n2));
	std::vector<std::vector<float> > h_yy(n1, std::vector<float>(n2));
	std::vector<std::vector<float> > h_zz(n1, std::vector<float>(n2));

	std::vector<float> h_x(dims);
	std::vector<float> h_y(dims);
	std::vector<float> h_z(dims);


	for (int i = 0; i < n1; ++i){
		for (int j = 0; j < n2; ++j){
			h_xx[i][j] = 1;
			h_yy[i][j] = 1;
		}
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

	arr.packVector(h_xx, h_x);
	arr.packVector(h_yy, h_y);

	clEnqueueWriteBuffer(queue, d_xx, CL_FALSE, 0, sizeof(float)*h_x.size(), &h_x, 0, NULL, NULL);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not write vector to buffer." << std::endl;
        std::cout << "OpenCL error code: " << std::endl;
        exit(1);
    }

	clEnqueueWriteBuffer(queue, d_yy, CL_FALSE, 0, sizeof(float)*h_y.size(), &h_y, 0, NULL, NULL);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not write vector to buffer." << std::endl;
        std::cout << "OpenCL error code: " << std::endl;
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
	
	err = clSetKernelArg(kernel, 3, sizeof(iters), &iters);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not set the integer kernel argument." << std::endl;
        std::cout << "OpenCL error code: " << err << std::endl;
        exit(1);
    }

	size_t work_units_per_kernel = dims;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &work_units_per_kernel, NULL, 0, NULL, NULL);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not execute the kernel." << std::endl;
        exit(1);
    }

	err = clEnqueueReadBuffer(queue, d_zz, CL_TRUE, 0, sizeof(float)*h_z.size(), &h_z, 0, NULL, NULL);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not read vector from the kernel." << std::endl;
        std::cout << "OpenCL error code: " << std::endl;
        exit(1);
    }

	arr.unPackVector(h_z, h_zz);

	arr.printVec2D(h_zz);

	gpu.freeDeviceMem(d_xx);
	gpu.freeDeviceMem(d_yy);
	gpu.freeDeviceMem(d_zz);
	gpu.freeKernel();
	gpu.freeCommandQueue();
	gpu.freeProgram();
	gpu.freeContext();

	return 0;

}
