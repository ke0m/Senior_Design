/*
 * File Name: "FlopsTestGPULoop.java"
 * Author: Joseph Jennings
 * Date: August 31, 2013
 * Description: This code is used to test the performance of summing two 2D arrays in parallel using the GPU.
 * 				It outputs a time value that measures how long it takes for the program to execute.
 */


//Import the JOCL libraries
import static org.jocl.CL.*;

import org.jocl.*;

	/*
	 * This is the code that is known as the kernel code and is sent to the GPU for execution. Essentially,
	 * this code sums the arrays d_xx d_yy and stores their output into the array d_zz. Each array is 1000x1000
	 * in size and the arrays are summed in total 502 times.
	 */

public class FlopsTestGPULoop {
	
	private static String kernelCode = 
			"__kernel void " +
			"arraysum(__global const float *d_xx,"+
			"		  __global const float *d_yy,"+
			"         __global float *d_zz)" +
			"{"+
			"     int i = get_global_id(0);"+
			"     d_zz[i] = d_xx[i] + d_yy[i];"+
			"	  for(int j = 0; j<250; j++){" +
			"			d_zz[i] += d_yy[i];	 "+
			"	   }						"+
			"}";
	
	public static void main(String args[]){

		/*  
         * These are the declarations of the OpenCL variables are described below:
         * cl_platform-id - Stores the types of platforms installed on the host.
         * cl_device_id - Stores the type of the device (GPU, CPU, etc.)
         * cl_context - Stores the context in which a command queue can be created.
         * cl_command_queue - Stores the command queue which governs how the GPU will
         *                    will execute the kernel.
         * cl_program - Stores the kernel code (which can be comprised of several kernels). Is compiled later its
         *              functions get packaged into kernels.
         * cl_kernel - The OpenCL data structure that represents kernels.
         */
		
		cl_platform_id platforms [] = new cl_platform_id[1];
		cl_device_id devices [] = new cl_device_id[1];
		cl_context context;
		cl_command_queue queue;
		cl_program program;
		cl_kernel kernel;

        /*
         * The err and errM variables will allow for checking for errors when the 
         * OpenCL functions are called. The logSize variable allocates the memory in order to store the output log created
         * by the compiler during the compilation of the kernel code. The buffer variable sb stores the 
         * log output in a buffer before then appending it to a string.
         */

		int err;
		int errM [] = new int[1];
		long logSize [] = new long[1];
		StringBuffer sb = new StringBuffer();

		
        /*
         * These Pointer objects allow for the a "pointer" datatype to be passed to the OpenCL functions such as
         * cl_EnqueueWriteBuffer and cl_EnqueueReadBuffer.
         */

		Pointer ph_xx = new Pointer();
		Pointer ph_yy = new Pointer();
		Pointer ph_zz = new Pointer();
		
		//This value determines the size of the nxn square array.
		int n = 1000;
		
		//The number of work items in each dimension of the data.
		long work_units_per_kernel[] = new long[]{n};
		long local_item_size [] = new long[]{1};
	
		//Declaring the	x,y, and z arrays
		float h_xx[][] = new float[n][n];
		float h_yy[][] = new float[n][n];
		float h_zz[][] = new float[n][n];

		/*
         * These three variables of the type cl_mem (memory object) are used as buffers and hold the data which will
         * be sent to the device and then once calculated sent back to the host.
         */
		
		cl_mem d_xx;
		cl_mem d_yy;
		cl_mem d_zz;
		
		//This for loop initializes the arrays on the host.
		for(int i = 0; i<n; i++){
			for(int j = 0; j<n; j++){
				
				h_xx[i][j] = i + j;
				h_yy[i][j] = i + j;
				
			}
			
		}
		
		// Obtains the Platform information installed on the host and stores into the argument "platforms"
		err = clGetPlatformIDs(1, platforms, null);
		if(err < 0){
			
			System.out.println("Error: Failed to locate platform.");
			System.exit(1);
		}

		// Obtains the device information (looking for specifically GPU devices) and stores it into the argument "devices"
		err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1, devices ,null);
		if(err < 0){
			System.out.println("Error: Failed to locate device.");
			System.exit(1);
		}		

		// Creates a context on the device and stores it into the "context" variable.
		context = clCreateContext(null, 1, devices, null, null, errM);
		if(errM[0] < 0){
			System.out.println("Error: Failed to create context");
			System.exit(1);
		}
		
		// Stores the kernel code (at this point only a Java String) into a program and stores it into the "program" variable.
		program = clCreateProgramWithSource(context, 1, new String[]{ kernelCode }, null, errM);
		if(errM[0] < 0){
			System.out.println("Error: Failed to create the program");
			System.exit(1);
		}
			
		//Compiles the program and stores the compiled code into the argument "program"
		err = clBuildProgram(program, 1, devices, null, null, null);
		if(err < 0){
			System.out.println("Error: Failed to build the program.");

            /*
             * Stores program build log into a String buffer.
             * First allocates the correct amount of memory in order to write the build log into a buffer.
             * Then it writes the build log to the argument "logData"
             */
			
			clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, null, logSize);
			byte logData[] = new byte[(int)logSize[0]];
			clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, logSize[0], Pointer.to(logData), null);
			sb.append(new String(logData, 0, logData.length - 1));
			sb.append("\n");
			String buildInfo = sb.toString();
			System.out.println(buildInfo);
			System.exit(1);
		}
		
		//From the compiled code in the program creates a kernel called "arraysum"
		kernel = clCreateKernel(program, "arraysum", errM);
		if(errM[0] < 0){
			System.out.println("Error: Failed to create the kernel.");
			System.exit(1);
		}
		

		//Creates a command queue and stores it into the variable "queue".
		queue = clCreateCommandQueue(context, devices[0], 0, errM);
		if(errM[0] < 0){
			System.out.println("Error: Failed to create the command queue.");
			System.exit(1);
		}
		

		//Creates a memory buffer. Three memory buffers were created. One for each array d_xx, d_yy, d_zz.
		d_xx = org.jocl.CL.clCreateBuffer(context, CL_MEM_READ_WRITE, Sizeof.cl_float*n, null, errM);
		if(errM[0] < 0){
			System.out.println("Error: Could not create the buffer d_xx.");
			System.exit(1);
		}
		
		d_yy = clCreateBuffer(context, CL_MEM_READ_WRITE, Sizeof.cl_float*n, null, errM);
		if(errM[0]  < 0){
			System.out.println("Error: Could not create the buffer d_yy.");
			System.exit(1);
		}
		
		d_zz = clCreateBuffer(context, CL_MEM_READ_WRITE, Sizeof.cl_float*n, null, errM);
		if(errM[0] < 0){
			System.out.println("Error: Could not create the buffer d_zz.");
			System.exit(1);
		}
		
		//This variable stores the initial time before the program is executed on the GPU.
		long startTime = System.nanoTime();
		

        /*
         * This for loop loops over the each row in the matrices x and y first writes the row to the device memory where 
         * the kernel arguments are then set and then then passed to the compiled kernel code already located on the device. 
         * Once executed, the results are then stored in the d_zz buffer and are read back to the host.
         */
		
		for(int i = 0; i < n; i++){
			
			//Writing the data from the host to the device
			err = clEnqueueWriteBuffer(queue, d_xx, CL_TRUE, 0, Sizeof.cl_float*n, ph_xx.to(h_xx[i]), 0, null, null);
			if(err < 0){
				System.out.println("Error: Could not write to buffer d_xx");
				System.exit(1);
			}
			
			err = clEnqueueWriteBuffer(queue, d_yy, CL_TRUE, 0, Sizeof.cl_float*n, ph_xx.to(h_yy[i]), 0, null, null);
			if(err < 0){
				System.out.println("Error: Could not write to buffer d_nn");
				System.exit(1);
			}
			

			//Setting the Kernel Arguments
			err = clSetKernelArg(kernel, 0, Sizeof.cl_mem, Pointer.to(d_xx));
			if(err < 0){
				System.out.println("Error: Could not set kernel argument h_xx");
				System.exit(1);	
			}
			
			err = clSetKernelArg(kernel, 1, Sizeof.cl_mem, Pointer.to(d_yy));
			if(err < 0){
				System.out.println("Error: Could not set kernel argument h_xx");
				System.exit(1);	
			}
			
			err = clSetKernelArg(kernel, 2, Sizeof.cl_mem, Pointer.to(d_zz));
			if(err < 0){
				System.out.println("Error: Could not set kernel argument h_zz");
				System.exit(1);	
			}
			

			//Executing the Kernel
			err = clEnqueueNDRangeKernel(queue, kernel, 1, null, work_units_per_kernel, null, 0, null, null);
			if(err < 0){
				System.out.println("Error: Could not execute the kernel");
				System.exit(1);
			}
			
			//Reading the Data from the device
			err = org.jocl.CL.clEnqueueReadBuffer(queue, d_zz, CL_TRUE, 0, n*Sizeof.cl_float, ph_zz.to(h_zz[i]), 0, null, null);
			if(err < 0){
				System.out.println("Error: Could not read the data from the kernel.");
				System.exit(1);
			}
			
			
		}
		
		//Now finding the difference between the end time and the start time in order to measure the total time taken to exectute
		//the program on the GPU.
		long elapsedTime = System.nanoTime() - startTime;
		//Printing the measured time to the screen.	
		System.out.println(elapsedTime);
	

		//The previously allocated memory is freed.
		clReleaseMemObject(d_xx);
		clReleaseMemObject(d_yy);
		clReleaseMemObject(d_zz);
		clReleaseKernel(kernel);
		clReleaseCommandQueue(queue);
		clReleaseProgram(program);
		clReleaseContext(context);
		
		
	}
	

}
