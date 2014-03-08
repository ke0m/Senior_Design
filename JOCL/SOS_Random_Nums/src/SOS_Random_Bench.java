import org.jocl.*;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.Random;

import edu.mines.jtk.mosaic.*;
import edu.mines.jtk.util.ArrayMath;
import edu.mines.jtk.util.Stopwatch;

public class SOS_Random_Bench {
	
	
	public static void main(String args[]) throws FileNotFoundException 
	{
		
		Random rand = new Random();
		Scanner in = new Scanner(System.in);
		Pointer phdata = new Pointer();
		Stopwatch sw = new Stopwatch();
		
		int RAND_MAX = 1;
		
		int n1 = 1024;
		int n2 = n1+2;// MUST be even
		int[] n1p = new int[1];
		n1p[0] = n1;
		int[] n2p = new int[1];
		n2p[0] = n2;
		int[] dims =  new int[1];
		dims[0] = n1*n2;
		int[] value = new int[1];
		value[0] = 0;
		int total = 2048;
		int numPoints = (int)(Math.log(total) / Math.log(2));
		float[] cpudata = new float[numPoints];
		float[] gpudata = new float[numPoints];
		
		cl_platform_id[] platforms = new cl_platform_id[1];
		cl_device_id[] devices = new cl_device_id[1];
		cl_context context;
	    cl_command_queue queue;
	    cl_program program;
	    cl_kernel kernel0, kernel1;
		int num_platforms;
		
		int err;
		int errM[] = new int[1];
		
		String fileName;
		String sourceStr;
		
		int programType;
		
		float e11, e12, e22;
		float r00, r01, r10, r11;
		float rs, ra, rb, sa, sb;
		float r1, r2, s1, s2;
		float alpha = 18.0f;
		float[] alphaCL = new float[1];
		alphaCL[0] = alpha;
		
		
		err = CL.clGetPlatformIDs(1, platforms, null);
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Failed to locate platform.");
			System.exit(1);
		}
		
		err = CL.clGetDeviceIDs(platforms[0], CL.CL_DEVICE_TYPE_GPU, 1, devices, null);
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Failed to locate device.");
			System.out.println("OpenCL Error Code: " + err);
			System.exit(1);
		}	
		
		context = CL.clCreateContext(null, 1, devices, null, null, errM);
		if(errM[0] != CL.CL_SUCCESS){
			System.out.println("Error: Failed to create the context.");
			System.out.println("OpenCL Error Code: " + err);
			System.exit(1);
		}
		
		fileName = "sosmoothing.cl";
		sourceStr = readFile(fileName);
		
		program = CL.clCreateProgramWithSource(context, 1, new String[]{ sourceStr },  null, errM);
		if(errM[0] != CL.CL_SUCCESS){
			System.out.println("Error: Failed to create the program");
			System.exit(1);
			}

		err = org.jocl.CL.clBuildProgram(program, 1, devices, "-cl-strict-aliasing -cl-unsafe-math-optimizations -cl-finite-math-only -cl-fast-relaxed-math -DMAC", null, null);
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Failed to build the program.");
			System.out.println(getProgramInfo(devices, program));
			System.exit(1);
		}
		
		kernel0 = CL.clCreateKernel(program, "setMem", errM);
		if(errM[0] != CL.CL_SUCCESS){
			System.out.println("Error: Failed to create the kernel.");
			System.out.println("OpenCL Error Code: " + errM[0]);
			System.exit(1);
		}

		kernel1 = CL.clCreateKernel(program, "soSmoothingNew", errM);
		if(errM[0] != CL.CL_SUCCESS){
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
		
		
		float[][] h_rbe = new float[n1][n2];
		float[][] h_sbe = new float[n1][n2];
		float[][] sbe   = new float[n1][n2];
		float[][] h_d11be = new float[n1][n2];
		float[][] h_d12be = new float[n1][n2];
		float[][] h_d22be = new float[n1][n2];
	
	
	
		for(int i = 0; i < n1; i++)
		{
			for(int j = 0; j < n2; j++)
			{
				h_rbe[i][j]   = rand.nextFloat()/RAND_MAX;
				h_d11be[i][j] = rand.nextFloat()/RAND_MAX;
				h_d12be[i][j] = rand.nextFloat()/RAND_MAX;
				h_d22be[i][j] = rand.nextFloat()/RAND_MAX;
				h_sbe[i][j]   = 0.0f;
				sbe[i][j]     = 0.0f;
				
			}
		}
		
		
		//Packing Arrays
		
		float[] h_rbe1 = new float[dims[0]];
		float[] h_sbe1 = new float[dims[0]];
		float[] h_d11be_1 = new float[dims[0]];
		float[] h_d12be_1 = new float[dims[0]];
		float[] h_d22be_1 = new float[dims[0]];
		
		int k = 0;
		for(int i = 0; i < n1; i++)
		{
			for(int j = 0; j < n2; j++)
			{
				h_rbe1[k]   = h_rbe[i][j];
				h_sbe1[k]   = h_sbe[i][j];
				h_d11be_1[k] = h_d11be[i][j];
				h_d12be_1[k] = h_d12be[i][j];
				h_d22be_1[k] = h_d22be[i][j];
				k++;
			}
		}
		
		int maxTime = 2;
		int cpucount = 0;
		sw.restart();
		while(sw.time() < maxTime)
		{
			for(int i2 = 1; i2 < n1; ++i2)
			{
				for(int i1 = 1; i1 < n2; ++i1)
				{

					e11 = alpha * h_d11be[i2][i1]; //fill matrices here with random numbers
					e12 = alpha * h_d12be[i2][i1]; //fill D matrix with random numbers
					e22 = alpha * h_d22be[i2][i1];
					r00 = h_rbe[i2  ][i1  ];
					r01 = h_rbe[i2  ][i1-1];
					r10 = h_rbe[i2-1][i1  ];
					r11 = h_rbe[i2-1][i1-1];
					rs = 0.25f*(r00+r01+r10+r11);
					ra = r00-r11;
					rb = r01-r10;
					r1 = ra-rb;
					r2 = ra+rb;
					s1 = e11*r1+e12*r2;
					s2 = e12*r1+e22*r2;
					sa = s1+s2;
					sb = s1-s2;
					sbe[i2  ][i1  ] += sa+rs;
					sbe[i2  ][i1-1] -= sb-rs;
					sbe[i2-1][i1  ] += sb+rs;
					sbe[i2-1][i1-1] -= sa-rs;
				}
			}
			
			cpucount++;
			
		}
		sw.stop();
		float cputime = (float)sw.time();
		
		//GPU Benchmark
		
		cl_mem d_s, d_r, d_d11, d_d12, d_d22;
		
		//Allocating memory on the GPU
		
		d_r = CL.clCreateBuffer(context, CL.CL_MEM_READ_ONLY, Sizeof.cl_float*(dims[0]), null, errM);
		if(errM[0] != CL.CL_SUCCESS){
			System.out.println("Error: Could not create the buffer.");
			System.out.println("OpenCL Error Code: " + errM[0]);
			System.exit(1);
		}
		
		d_d11 = CL.clCreateBuffer(context, CL.CL_MEM_READ_ONLY, Sizeof.cl_float*(dims[0]), null, errM);
		if(errM[0] != CL.CL_SUCCESS){
			System.out.println("Error: Could not create the buffer.");
			System.out.println("OpenCL Error Code: " + errM[0]);
			System.exit(1);
		}
		
		d_d12 = CL.clCreateBuffer(context, CL.CL_MEM_READ_ONLY, Sizeof.cl_float*(dims[0]), null, errM);
		if(errM[0] != CL.CL_SUCCESS){
			System.out.println("Error: Could not create the buffer.");
			System.out.println("OpenCL Error Code: " + errM[0]);
			System.exit(1);
		}
		
		d_d22 = CL.clCreateBuffer(context, CL.CL_MEM_READ_ONLY, Sizeof.cl_float*(dims[0]), null, errM);
		if(errM[0] != CL.CL_SUCCESS){
			System.out.println("Error: Could not create the buffer.");
			System.out.println("OpenCL Error Code: " + errM[0]);
			System.exit(1);
		}
		
		d_s = CL.clCreateBuffer(context, CL.CL_MEM_READ_WRITE, Sizeof.cl_float*(n1+1)*(n2+1), null, errM);
		if(errM[0] != CL.CL_SUCCESS){
			System.out.println("Error: Could not create the buffer.");
			System.out.println("OpenCL Error Code: " + errM[0]);
			System.exit(1);
		}
		
		//Copy the data to the GPU
		
		err = CL.clEnqueueWriteBuffer(queue, d_r, CL.CL_FALSE, 0, Sizeof.cl_float*dims[0], phdata.to(h_rbe1), 0, null, null);
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not write data to buffer");
			System.out.println("OpenCL Error Code: " + err);
			System.exit(1);
		}
		
		
		
		err = CL.clEnqueueWriteBuffer(queue, d_d11, CL.CL_FALSE, 0, Sizeof.cl_float*dims[0], phdata.to(h_d11be_1), 0, null, null);
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not write data to buffer");
			System.out.println("OpenCL Error Code: " + err);
			System.exit(1);
		}	
		
		err = CL.clEnqueueWriteBuffer(queue, d_d12, CL.CL_FALSE, 0, Sizeof.cl_float*dims[0], phdata.to(h_d12be_1), 0, null, null);
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not write data to buffer");
			System.out.println("OpenCL Error Code: " + err);
			System.exit(1);
		}	
		
		err = CL.clEnqueueWriteBuffer(queue, d_d22, CL.CL_FALSE, 0, Sizeof.cl_float*dims[0], phdata.to(h_d22be_1), 0, null, null);
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not write data to buffer");
			System.out.println("OpenCL Error Code: " + err);
			System.exit(1);
		}	
		
		
		// Calling the kernel
		
		err = CL.clSetKernelArg(kernel0, 0, Sizeof.cl_mem, Pointer.to(d_s));
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not set kernel argument.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);	
		}
		
		err = CL.clSetKernelArg(kernel0, 1, Sizeof.cl_int, Pointer.to(value));
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not set kernel argument.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);	
		}
		
		err = CL.clSetKernelArg(kernel0, 2, Sizeof.cl_int, Pointer.to(dims));
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not set kernel argument.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);	
		}
		
		long[] oned_global_work_size = new long[]{dims[0]};
		err = CL.clEnqueueNDRangeKernel(queue, kernel0, 1, null, oned_global_work_size, null, 0, null, null);
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not execute the kernel");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);
		}
		
		long[] local_group_size = new long[]{32, 32};
		long[] mapped_n1 = new long[]{n2/2};
		long[] mapped_n2 = new long[]{n1/2};
		long[] global_group_size_block = new long[]{(long)Math.ceil(mapped_n1[0]/local_group_size[0] + 1)*local_group_size[0], (long)Math.ceil(mapped_n2[0]/local_group_size[0] + 1) * local_group_size[1]};
		
		err = CL.clSetKernelArg(kernel1, 0, Sizeof.cl_mem, phdata.to(d_r));
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not set kernel argument.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);	
		}
		
		
		err = CL.clSetKernelArg(kernel1, 1, Sizeof.cl_mem, phdata.to(d_d11));
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not set kernel argument.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);	
		}
		
		err = CL.clSetKernelArg(kernel1, 2, Sizeof.cl_mem, phdata.to(d_d12));
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not set kernel argument.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);	
		}
		
		
		err = CL.clSetKernelArg(kernel1, 3, Sizeof.cl_mem, phdata.to(d_d22));
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not set kernel argument.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);	
		}
		
		err = CL.clSetKernelArg(kernel1, 4, Sizeof.cl_mem, phdata.to(d_s));
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not set kernel argument.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);	
		}
		
		err = CL.clSetKernelArg(kernel1, 5, Sizeof.cl_float, phdata.to(alphaCL));
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not set kernel argument.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);	
		}
		
		err = CL.clSetKernelArg(kernel1, 6, Sizeof.cl_float, phdata.to(n1p));
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not set kernel argument.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);	
		}
		
		err = CL.clSetKernelArg(kernel1, 7, Sizeof.cl_float, phdata.to(n2p));
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not set kernel argument.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);	
		}
		
		

		
		int gpucount = 0;
		sw.restart();
		while(sw.time() < maxTime)
		{
		

			
			
			for(int offsetx = 0; offsetx < 2; ++offsetx)
			{
				for(int offsety = 0; offsety < 2; ++offsety)
				{
					
					int[] offsetxp = new int[1];
					offsetxp[0] = offsetx;
					err = CL.clSetKernelArg(kernel1, 8, Sizeof.cl_float, phdata.to(offsetxp));
					if(err != CL.CL_SUCCESS){
						System.out.println("Error: Could not set kernel argument.");
						System.out.println("OpenCL error code: " + err);
						System.exit(1);	
					}
					
					int[] offsetyp = new int[1];
					offsetyp[0] = offsety;
					err = CL.clSetKernelArg(kernel1, 9, Sizeof.cl_float, phdata.to(offsetyp));
					if(err != CL.CL_SUCCESS){
						System.out.println("Error: Could not set kernel argument.");
						System.out.println("OpenCL error code: " + err);
						System.exit(1);	
					}
					
					err = CL.clEnqueueNDRangeKernel(queue, kernel1, 2, null, global_group_size_block, local_group_size, 0, null, null);
					if(err != CL.CL_SUCCESS){
						System.out.println("Error: Could not execute the kernel");
						System.out.println("OpenCL error code: " + err);
						System.exit(1);
					}
					

				}
			}
			
			
			
			
			
			
			gpucount++;
		}
		
		
		sw.stop();
		float gputime = (float)sw.time();
		
		err = CL.clEnqueueReadBuffer(queue, d_s, CL.CL_TRUE, 0, Sizeof.cl_float*(n1+1)*(n2+1), phdata.to(h_sbe1), 0, null, null);
		if(err != CL.CL_SUCCESS){
			System.out.println("Error: Could not read the data from the kernel.");
			System.out.println("OpenCL error code: " + err);
			System.exit(1);
		}
		

		
		
		CL.clReleaseMemObject(d_r);
		CL.clReleaseMemObject(d_s);
		CL.clReleaseMemObject(d_d11);
		CL.clReleaseMemObject(d_d12);
		CL.clReleaseMemObject(d_d22);
		CL.clReleaseKernel(kernel0);
		CL.clReleaseKernel(kernel1);
		CL.clReleaseCommandQueue(queue);
		CL.clReleaseProgram(program);
		CL.clReleaseContext(context);
		
		//unpacking the array
		
		k = 0;
		for (int i = 0; i < n1; i++)
		{
			for (int j = 0; j < n2; j++)
			{
				h_sbe[i][j] = h_sbe1[k];
				k++;
	
			}
		}
		
		//Displaying the results
		
		float cpucountf = (float)(cpucount);
		float gpucountf = (float)(gpucount);
		float numflops = 29.0f;
		
		
		float cpumflops = (cpucountf * numflops * (n1-1) * (n2-1) * (float)1.0e-6)/cputime;
		float gpumflops = (gpucountf * numflops * (n1-1) * (n2-1) * (float)1.0e-6)/gputime;
		
		System.out.println("CPU Benchmark: " + cpumflops + " MFLOPS");
		System.out.println("GPU Benchmark: " + gpumflops + " MFLOPS");
		
	}
	
	
	
	/**
	 * Reads a text file into a String.
	 * @param file the text file to be read into the String
	 * @return the string containing the text from the text file
	 */
	public static String readFile(String file) throws FileNotFoundException{

		String text = new Scanner( new File(file)).useDelimiter("\\A").next();

		return text;

	}
	
	public static String getProgramInfo(cl_device_id[] devices, cl_program program){
		
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
