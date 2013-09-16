import static org.jocl.CL.*;
import org.jocl.*;


public class GPU {
	
	
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
	
	/*
	 * Takes data from host and writes it to the device for processing
	 * Takes as parameters: specified device variable (of type cl_mem),
	 * host variable, dimensions of data to be sent.
	 */
	public void writeToDevice(cl_mem ddata, float[] hdata, int dataDims){
		err = clEnqueueWriteBuffer(queue, ddata, CL_TRUE, 0, Sizeof.cl_float*dataDims, phdata.to(hdata), 0, null, null);
		if(err < 0){
			System.out.println("Error: Could write data to buffer");
			System.exit(1);
		}	
	}

	public void setKernelArg(cl_mem ddata, int kernelNum){
		err = clSetKernelArg(kernel, kernelNum, Sizeof.cl_mem, Pointer.to(ddata));
		if(err < 0){
			System.out.println("Error: Could not set kernel argument.");
			System.exit(1);	
		}
	}
	
	
	public void readFromDevice(cl_mem ddata, float[] hdata, int dataDims){
		err = org.jocl.CL.clEnqueueReadBuffer(queue, ddata, CL_TRUE, 0, dataDims*Sizeof.cl_float, phdata.to(hdata), 0, null, null);
		if(err < 0){
			System.out.println("Error: Could not read the data from the kernel.");
			System.exit(1);
		}
	}
	
	public cl_platform_id[] getPlatform(){
	        
	        return platforms;
	    }
	    
	public cl_device_id[] getDevice(){
	        
	        return devices;
	    }
	    
	public cl_context getContext(){
	        
	        return context;
	    }
	    
	public cl_command_queue getQueue(){
	        
	        return queue;
	    }
	    
	public cl_kernel getKernel(){
	        
	        return kernel;
	    }
	    
	public cl_program  getProgram(){
	        
	        return program;
	    }

	public int getErr(){
	        
	        return err;
	    }
	
	public int[] getErrM(){
		
		return errM;
	}
	
	public cl_mem createFloatBuffer(int dataDims){
		cl_mem buff;
		buff = clCreateBuffer(context, CL_MEM_READ_WRITE, Sizeof.cl_float*dataDims, null, errM);
		if(errM[0]  < 0){
			System.out.println("Error: Could not create the buffer.");
			System.exit(1);
		}
		return buff;	
	}
	
	public void executeKernel(int dataDims){
		long[] work_units_per_kernel = new long[]{dataDims};
		err = clEnqueueNDRangeKernel(queue, kernel, 1, null, work_units_per_kernel, null, 0, null, null);
		if(err < 0){
			System.out.println("Error: Could not execute the kernel");
			System.exit(1);
		}
		
	}
	
	public void setRunOnGPU(boolean gpuBool){
		
		if (gpuBool == false)
			runOnGPU = false;
	
	}
	
	

	
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

}
