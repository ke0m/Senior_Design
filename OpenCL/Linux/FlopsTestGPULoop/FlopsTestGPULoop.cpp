/*
 * File Name: "FlopsTestGPU.cpp"
 * Kernel Code: "flopstestloop.cl"
 * Author: Joseph Jennings
 * Date: August 17, 2013
 * Description: This code uses the 2D array sum example in order to compare the performance of GPU programming
 * 				and CPU programming. It allows us to see how many FLOPs are required in order to make
 * 				programming the GPU useful.
 */

//Include the OpenCL and the I/O header files
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <CL/opencl.h>
#include <sys/time.h>

int main(int argc, const char * argv[])
{
 

	
	 //First we set the variables for measuring performance.
	 
	 struct timeval tim1, tim2;	                
	 uint64_t time;
	 
	 //Calling the function "gettimeofday" to measure the time before the program executes.
	 gettimeofday(&tim1, NULL);
		                

	/*
	 * These are the declarations of the OpenCL structures are described below:
	 * cl_platform-id - Stores the types of platforms installed on the host.
	 * cl_device_id - Stores the type of the device (GPU, CPU, etc.)
	 * cl_context - Stores the context in which a command queue can be created.
	 * cl_command_queue - Stores the command queue which governs how the GPU will
	 *                    will execute the kernel.
	 * cl_program - Stores the kernel code (which can be comprised of several kernels). Is compiled later its
	 * 				functions get packaged into kernels.
	 * cl_kernel - The OpenCL data structure that represents kernels.
	 */

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
	cl_program program;
    cl_kernel kernel;
	
	//A cl_int used to store error flags that are returned if OpenCL function does not execute properly. 
	cl_int err;

    /*
	 * A file object and buffers used to store the input kernel code as well as allocate the memory for the kernel code 
	 * and the output log from the compiler during the compilation of the kernel code.
	 */
    
	FILE *program_handle;
    char *program_buffer, *program_log;
	size_t program_size, log_size;

	//The number of work items in each dimension of the data.
    size_t work_units_per_kernel;
    
	//This value determines the size of the nxn (square) array.
    int n = 1000;
    
	//Allocating the memory for the nxn arrays of floats.
    float **h_xx = (float**)malloc(sizeof(float*)*n);
    float **h_yy = (float**)malloc(sizeof(float*)*n);
    float **h_zz = (float**)malloc(sizeof(float*)*n);
    
    for(int i = 0; i<n; i++){
        h_xx[i] = (float*)malloc(sizeof(float)*n);
        h_yy[i] = (float*)malloc(sizeof(float)*n);
        h_zz[i] = (float*)malloc(sizeof(float)*n);
        
		//Initializing the arrays.
        for(int j = 0; j<n; j++){
            
            h_xx[i][j] = i+j;
            h_yy[i][j] = i+j;
            
        }
       
    }
    	
	/*
	 * These three variables of the type cl_mem (memory object) are used as buffers and hold the data which will
	 * be sent to the device and then once calculated sent back to the host.
	 */

    cl_mem d_xx;
    cl_mem d_yy;
    cl_mem d_zz;

	
    
    // Obtains the Platform information installed on the host and stores into the memory location of the variable "platform"
    err = clGetPlatformIDs(1, &platform, NULL);
    if(err != CL_SUCCESS){
        
        std::cout << "Error: Failed to locate Platform." << std::endl;
        exit(1);
    }
    
	// Obtains the device information (looking for specifically GPU devices) and stores it into the memory location of the variable "device"
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if(err != CL_SUCCESS){
        printf("Error: Failed to locate Device.");
        exit(1);
    }
    
    // Creates a context on the device and stores it into the "context" variable.  
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create context." << std::endl;
        exit(1);
    }
    
    /*
	 * The following code stores the file "arraySum.cl" into the FILE object "program_handle". It then determines the size
	 * of the file and reads the content into the variable "program_buffer".
	 */

	program_handle = fopen("flopstestloop.cl", "r");
    if(!program_handle){
        std::cout << "Error: Failed to Load Kernel" << std::endl;
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char*)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);
    
    // Stores the kernel code into a program and stores it into the "program" variable.
	program = clCreateProgramWithSource(context, 1, (const char **)&program_buffer, (const size_t *)&program_size, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the program" << std::endl;
        exit(1);
    }
    
    free(program_buffer);

    //Compiles the program and stores the compiled code into the argument "program"
	err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not compile the program" << std::endl;

		/*
		 * The following code first allocates the correct amount of memory in order to store the output of the compilers
		 * build log and then it stores this log into the buffer "program_log". Finally it prints this buffer to the
		 * screen.
		 */

        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        program_log = (char*)malloc(log_size+1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size+1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }
    
	//From the compiled code in the program creates a kernel called "arraysum"
    kernel = clCreateKernel(program, "arraysum", &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the kernel" << std::endl;
        exit(1);
    }
    
	//Creates a command queue and stores it into the variable "queue".
    queue = clCreateCommandQueue(context, device, 0, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the queue" << std::endl;
        exit(1);
    }
    
    //Creating the Device memory buffers. These will be used to transfer data from the host to the device and vice versa.
    d_xx = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*n, NULL, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the buffer d_xx" << std::endl;
        exit(1);
    }
    
    d_yy = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*n, NULL, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the buffer d_yy" << std::endl;
        exit(1);
    }
    
    d_zz = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*n, NULL, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the buffer d_zz" << std::endl;
        exit(1);
    }
    
   /*
	* This for loop loops over the each row in the matrices x and y first writes the row to the device memory where 
	* the kernel arguments are then set and then then passed to the compiled kernel code already located on the device. 
	* Once executed, the results are then stored in the d_zz buffer and are read back to the host.
	*/

    for(int i = 0; i<n; i++)
    {
        //Writing the data from the host to the device
        err = clEnqueueWriteBuffer(queue, d_xx, CL_TRUE, 0, sizeof(float)*n, h_xx[i], 0, NULL, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not write to buffer d_xx" << std::endl;
            exit(1);
        }
        
        err = clEnqueueWriteBuffer(queue, d_yy, CL_TRUE, 0, sizeof(float)*n, h_yy[i], 0, NULL, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not write to buffer d_yy" << std::endl;
            exit(1);
        }
    
        //Setting the Kernel Arguments
        err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_xx);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not set kernel argument h_xx." << std::endl;
            exit(1);
        }
    
        err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_yy);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not set kernel argument h_yy." << std::endl;
            exit(1);
        }
    
        err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_zz);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not set kernel argument h_zz." << std::endl;
        }
    
        work_units_per_kernel = n;
    
        //Executing the Kernel
        err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &work_units_per_kernel, NULL, 0, NULL, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not execute kernel." << std::endl;
            exit(1);
        }
    
        //Reading the Data from the Kernel
        err = clEnqueueReadBuffer(queue, d_zz, CL_TRUE, 0, n*(sizeof(float)), h_zz[i], 0, NULL, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not read data from kernel." << std::endl;
            exit(1);
        }
        
    }

    //Measuring the time after the OpenCL code has executed and has been copied back to the host.
	gettimeofday(&tim2, NULL);
	//Finding the difference between the two measured times.
	time = tim2.tv_sec - tim1.tv_sec;
	//Displaying the elapsed time in seconds.
	std::cout << time + (tim2.tv_usec - tim1.tv_usec)/1000000.00 << std::endl;

    //The previously allocated memory is freed.
    clReleaseMemObject(d_xx);
    clReleaseMemObject(d_yy);
    clReleaseMemObject(d_zz);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    
    return 0;
}

