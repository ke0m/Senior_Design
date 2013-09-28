import static org.jocl.CL.*;
import org.jocl.*;

/**
 * A class that contains all of the methods and variables needed to initialize the GPU for OpenCL
 * programming. Allows for the basic GPU operations using OpenCL such as:
 * 
 * 1. Locating and initializing an OpenCL platform
 * 2. Locating and initializing a specific CPU or GPU device
 * 3. Create a context on the device
 * 4. Building an OpenCL program
 * 5. Creating an kernel to be run on the GPU
 * 6. Creating a command queue allowing the GPU to execute the kernel
 * 7. Reading and writing to the GPU or CPU device
 * 8. Setting kernel arguments
 * 9. Executing the kernel on the GPU or CPU
 * 
 * @author Joseph Jennings
 * @version 2013.09.28
 *
 */


public class GPU {

	
    /**
     * Class variables: 
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
	
	cl_platform_id[] platforms = new cl_platform_id[1];
	cl_device_id[] devices = new cl_device_id[1];
	cl_context context;
	cl_command_queue queue;
	int err;
	int[] errM = new int[1];
	long[] logSize = new long[1];
	StringBuffer sb = new StringBuffer();
	Pointer phdata= new Pointer();

	int dataDims;
	cl_program program;
	cl_kernel kernel;

	String kernelCode;
	String kernelName;
	
	boolean runOnGPU = true;
	
	/**
	 * Constructs a GPU object for the specified code to be executed on the GPU, the 
	 * specified name of the kernel code as written in the kernel code, as well the 
	 * specified location of where the code is to be run (on the CPU or GPU).
	 * 
	 * @param kernelStringName the String variable containing the code to be sent to the kernel
	 * @param nameOfKernel the name of the kernel as written in the kernel code
	 * @param gpuVCpu a boolean flag indicating the whether the code is to be sent to the CPU or GPU
	 */
	public GPU(String kernelStringName, String nameOfKernel, boolean gpuVCpu){
		
		err = 0;
		errM[0] = 0;
		kernelCode = kernelStringName;
		kernelName = nameOfKernel;
		setRunOnGPU(gpuVCpu);
		platforms = setPlatform();
		devices = setDevice();
		context = createContext();
		program = buildProgram();
		kernel = createKernel();
		queue = createQueue();
		
	}
	
	
	/**
	 * Gets an array of platform ids of the OpenCL platform on the current machine.
	 * @return an array of OpenCL platform ids.
	 */
	public cl_platform_id[] getPlatform(){
	        
	        return platforms;
	    }
	    
	/**
	 * Gets an array of OpenCL device ids for the device specified in the constructor (CPU or GPU)
	 * @return an array of OpenCL device ids.
	 */
	public cl_device_id[] getDevice(){
	        
	        return devices;
	    }
	    
	/**
	 * Gets an OpenCL context.
	 * @return an OpenCL context
	 */
	public cl_context getContext(){
	        
	        return context;
	    }
	    
	/**
	 * Gets an OpenCL command queue
	 * @return an OpenCL command queue
	 */
	public cl_command_queue getQueue(){
	        
	        return queue;
	    }
	    
	/**
	 * Gets the created OpenCL kernel
	 * @return an OpenCL kernel
	 */
	public cl_kernel getKernel(){
	        
	        return kernel;
	    }
	    
	/**
	 * Gets a compiled OpenCL program
	 * @return an OpenCL program
	 */
	public cl_program  getProgram(){
	        
	        return program;
	    }

	/**
	 * Gets an OpenCL integer error message
	 * @return an integer error code
	 */
	public int getErr(){
	        
	        return err;
	    }
	
	/**
	 * Gets an OpenCL array error message
	 * @return an integer array containing an error code
	 */
	public int[] getErrM(){
		
		return errM;
	}
	
	/**
	 * Creates an OpenCL memory buffer for a float type on the device.
	 * @param dataDims the dimensions of the data to be passed sent to the GPU (ie. the size of the float matrix)
	 * @return an OpenCL memory buffer on the device
	 */
	public cl_mem createFloatBuffer(int dataDims){
		cl_mem buff;
		buff = clCreateBuffer(context, CL_MEM_READ_WRITE, Sizeof.cl_float*dataDims, null, errM);
		if(errM[0]  < 0){
			System.out.println("Error: Could not create the buffer.");
			System.exit(1);
		}
		return buff;	
	}
	
	/**
	 * Writes data contained within an OpenCL buffer to the device.
	 * @param ddata the OpenCL buffer created on the device
	 * @param hdata the host data that will be sent to the specified OpenCL buffer on the device
	 * @param dataDims the dimensions of the host data to be sent to the OpenCL buffer
	 */
	public void writeToDevice(cl_mem ddata, float[] hdata, int dataDims){
		err = clEnqueueWriteBuffer(queue, ddata, CL_TRUE, 0, Sizeof.cl_float*dataDims, phdata.to(hdata), 0, null, null);
		if(err < 0){
			System.out.println("Error: Could not write data to buffer");
			System.exit(1);
		}	
	}
	
	/**
	 * 	Reads the data from an OpenCL buffer on the device
	 * @param ddata the memory buffer containing the data on the device
	 * @param hdata the host buffer that where the device data will be read to.
	 * @param dataDims the size of the data to be read
	 */
	public void readFromDevice(cl_mem ddata, float[] hdata, int dataDims){
		err = org.jocl.CL.clEnqueueReadBuffer(queue, ddata, CL_TRUE, 0, dataDims*Sizeof.cl_float, phdata.to(hdata), 0, null, null);
		if(err < 0){
			System.out.println("Error: Could not read the data from the kernel.");
			System.exit(1);
		}
	}
	
	/**
	 * Sets the kernel arguments on the device
	 * @param ddata the OpenCL memory buffer that has been created on the device
	 * @param kernelNum the number of kernel argument set. If this is the first time you
	 * 					calling this function then this number will be 0. The second time
	 * 					it will be 1, etc.
	 */
	public void setKernelArg(cl_mem ddata, int kernelNum){
		err = clSetKernelArg(kernel, kernelNum, Sizeof.cl_mem, Pointer.to(ddata));
		if(err < 0){
			System.out.println("Error: Could not set kernel argument.");
			System.exit(1);	
		}
	}
	
	/**
	 * Executes the kernel code on the device
	 * @param dataDims the dimensions of the data that have been sent to the device
	 */
	public void executeKernel(int dataDims){
		long[] work_units_per_kernel = new long[]{dataDims};
		err = clEnqueueNDRangeKernel(queue, kernel, 1, null, work_units_per_kernel, null, 0, null, null);
		if(err < 0){
			System.out.println("Error: Could not execute the kernel");
			System.exit(1);
		}
		
	}

	/**
	 * Tells the device to either run the code on the CPU or the GPU depending on the specified boolean flag.
	 * @param gpuBool a boolean argument that if false, will not execute the code on the GPU but rather the CPU
	 */
	public void setRunOnGPU(boolean gpuBool){
		
		if (gpuBool == false)
			runOnGPU = false;
	
	}
	
	/**
	 * Frees the memory occupied by the OpenCL device buffer
	 * @param ddata the OpenCL device buffer
	 */
	public void freeDeviceMem(cl_mem ddata){
		
		org.jocl.CL.clReleaseMemObject(ddata);
	}
	
	/**
	 * Frees the memory occupied by the kernel on the device
	 */
	public void freeKernel(){
		
		org.jocl.CL.clReleaseKernel(kernel);
	}
	
	/**
	 * Frees the memory occupied by the command queue on the device
	 */
	public void freeCommandQueue(){
		
		org.jocl.CL.clReleaseCommandQueue(queue);
	}
	
	/**
	 * Frees the memory occupied by the program on the device
	 */
	public void freeProgram(){
		
		org.jocl.CL.clReleaseProgram(program);
	}
	
	/**
	 * Frees the memory occupied by the context on the device.
	 */
	public void freeContext(){
		
		org.jocl.CL.clReleaseContext(context);
	}

	//TODO: Instead of passing the data dimensions parameter to many of these methods,
	//      it might make more sense for these methods to know the size already just from reading
	//		the size of the device buffers
	
	
	
	//////////////////////////////////////////////////////////////////////////////
	// private
	
	private cl_platform_id[] setPlatform(){
		
		err = clGetPlatformIDs(1, platforms, null);
		if(err < 0){
			
			System.out.println("Error: Failed to locate platform.");
			System.exit(1);
		}

		return platforms;
	}

	private cl_device_id[] setDevice(){
		if (runOnGPU == true){
			err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1, devices ,null);
			if(err < 0){
				System.out.println("Error: Failed to locate device.");
				System.exit(1);
			}	
			
		}
		else if (runOnGPU == false){
			err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 1, devices ,null);
			if(err < 0){
				System.out.println("Error: Failed to locate device.");
				System.exit(1);
			}	
			
		}
	
		return devices;
		
	}

	
	
	private cl_context createContext(){
		
		context = clCreateContext(null, 1, devices, null, null, errM);
		if(errM[0] < 0){
			System.out.println("Error: Failed to create context");
			System.exit(1);
		}
		
		return context;
	}
	

	private String getProgramInfo(){
		System.out.println("Error: Failed to build the program.");
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, null, logSize);
		byte[] logData = new byte[(int)logSize[0]];
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, logSize[0], Pointer.to(logData), null);
		sb.append(new String(logData, 0, logData.length - 1));
		sb.append("\n");
		String buildInfo = sb.toString();
		
		return buildInfo;
	}

	
	private cl_program buildProgram(){
		
		program = clCreateProgramWithSource(context, 1, new String[]{ kernelCode }, null, errM);
		if(errM[0] < 0){
			System.out.println("Error: Failed to create the program");
			System.exit(1);
		}
		
		err = clBuildProgram(program, 1, devices, null, null, null);
		if(err < 0){
			System.out.println("Error: Failed to build the program.");
			System.out.println(getProgramInfo());
			System.exit(1);
		}
	
		return program;
	
	}


	private cl_kernel createKernel(){
		
		kernel = clCreateKernel(program, kernelName, errM);
		if(errM[0] < 0){
			System.out.println("Error: Failed to create the kernel.");
			System.exit(1);
		}
		
		return kernel;
	}

	private cl_command_queue createQueue(){
		
		queue = clCreateCommandQueue(context, devices[0], 0, errM);
		if(errM[0] < 0){
			System.out.println("Error: Failed to create the command queue.");
			System.exit(1);
		}
		
		return queue;
	}
	

}
