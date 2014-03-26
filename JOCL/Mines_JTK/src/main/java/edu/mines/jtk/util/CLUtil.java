package edu.mines.jtk.util;

import org.jocl.*;

import static org.jocl.CL.*;

//TODO: Add the capablitity to create multiple kernels from one string


public class CLUtil {

	public cl_platform_id[] platforms = new cl_platform_id[1];
	public cl_device_id[] devices = new cl_device_id[1];
	public cl_context context;
	public cl_command_queue queue;
	int err;
	int[] errM = new int[1];

	public cl_program program;
	public cl_kernel kernel;

	String kernelCode;
	String kernelName;
	
	public CLUtil(String sourceStr, String nameOfKernel){

		
		err = clGetPlatformIDs(1, platforms, null);
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Failed to locate platform.");
			System.exit(1);
		}
		
		err = clGetDeviceIDs(platforms[0], CL.CL_DEVICE_TYPE_GPU, 1, devices, null);
		if(err != CL_SUCCESS){
			System.out.println("Error: Failed to locate device.");
			System.out.println("OpenCL Error Code: " + err);
			System.exit(1);
		}	
		
		context = clCreateContext(null, 1, devices, null, null, errM);
		if(errM[0] != CL_SUCCESS){
			System.out.println("Error: Failed to create the context.");
			System.out.println("OpenCL Error Code: " + err);
			System.exit(1);
		}
		
		program = clCreateProgramWithSource(context, 1, new String[]{ sourceStr },  null, errM);
		if(errM[0] != CL_SUCCESS){
			System.out.println("Error: Failed to create the program");
			System.exit(1);
		}
	
		err = clBuildProgram(program, 1, devices, "-cl-strict-aliasing -cl-unsafe-math-optimizations -cl-finite-math-only -cl-fast-relaxed-math -DMAC", null, null);
		if(err != CL_SUCCESS){
			System.out.println("Error: Failed to build the program.");
			System.out.println(getProgramInfo(devices, program));
			System.exit(1);
		}
		
		kernel = CL.clCreateKernel(program, nameOfKernel, errM);
		if(errM[0] != CL_SUCCESS){
			System.out.println("Error: Failed to create the kernel.");
			System.out.println("OpenCL Error Code: " + errM[0]);
			System.exit(1);
		}
	
		
		queue = CL.clCreateCommandQueue(context, devices[0], 0, errM);
		if(errM[0] != CL.CL_SUCCESS){
			System.out.println("Error: Failed to create the command queue.");
			System.out.println("OpenCL error code: " + errM[0]);
			System.exit(1);
		}
	}


	
	/////////////Private Methods
	
	  private static String getProgramInfo(cl_device_id[] devices, cl_program program){
			
			StringBuffer sb = new StringBuffer();
			long[] logSize = new long[1];
			System.out.println("Error: Failed to build the program.");
			org.jocl.CL.clGetProgramBuildInfo(program, devices[0], org.jocl.CL.CL_PROGRAM_BUILD_LOG, 0, null, logSize);
			byte[] logData = new byte[(int)logSize[0]];
			org.jocl.CL.clGetProgramBuildInfo(program, devices[0], org.jocl.CL.CL_PROGRAM_BUILD_LOG, logSize[0], Pointer.to(logData), null);
			sb.append(new String(logData, 0, logData.length - 1));
			sb.append("\n");
			String buildInfo = sb.toString();

			return buildInfo;
		}
	
	
}
