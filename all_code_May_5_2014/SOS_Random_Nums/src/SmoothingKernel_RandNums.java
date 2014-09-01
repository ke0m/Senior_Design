import org.jocl.*;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.Random;

import edu.mines.jtk.mosaic.*;
import edu.mines.jtk.util.ArrayMath;
import edu.mines.jtk.util.Stopwatch;

public class SmoothingKernel_RandNums {
	
	public static void main(String args[]) throws FileNotFoundException {
		
		
		Random rand = new Random();
		Scanner in = new Scanner(System.in);
		Pointer phdata = new Pointer();
		Stopwatch sw = new Stopwatch();
		
		int RAND_MAX = 1;
		
		int n1 = 512;
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

		err = org.jocl.CL.clBuildProgram(program, 1, devices, null, null, null);
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
		
		//don't make this global
		//Declaring and Initializing Arrays
		
		
		System.out.print("Please type: (1) Generate Image (2) Perform a Benchmark (3) Perfplot ");
		programType = in.nextInt();
		
		//SOS Image Creation
		
			//Image Data Creation on the CPU
		if(programType == 1)
		{
			
			float[][] h_ri = new float[n1][n2];
			float[][] h_si = new float[n1][n2];
			float[][] si   = new float[n1][n2];
			float[][] h_d11i = new float[n1][n2];
			float[][] h_d12i = new float[n1][n2];
			float[][] h_d22i = new float[n1][n2];
		
		
		
			for(int i = 0; i < n1; i++)
			{
				for(int j = 0; j < n2; j++)
				{
					h_ri[i][j]   = rand.nextFloat()/RAND_MAX;
					h_d11i[i][j] = rand.nextFloat()/RAND_MAX;
					h_d12i[i][j] = rand.nextFloat()/RAND_MAX;
					h_d22i[i][j] = rand.nextFloat()/RAND_MAX;
					h_si[i][j]   = 0.0f;
					si[i][j]     = 0.0f;
					
				}
			}
			
			
			//Packing Arrays
			
			float[] h_ri1 = new float[dims[0]];
			float[] h_si1 = new float[dims[0]];
			float[] h_d11i_1 = new float[dims[0]];
			float[] h_d12i_1 = new float[dims[0]];
			float[] h_d22i_1 = new float[dims[0]];
			
			int k = 0;
			for(int i = 0; i < n1; i++)
			{
				for(int j = 0; j < n2; j++)
				{
					h_ri1[k]   = h_ri[i][j];
					h_si1[k]   = h_si[i][j];
					h_d11i_1[k] = h_d11i[i][j];
					h_d12i_1[k] = h_d12i[i][j];
					h_d22i_1[k] = h_d22i[i][j];
					k++;
				}
			}
		
			for(int i2 = 1; i2 < n1; ++i2)
			{
				for(int i1 = 1; i1 < n2; ++i1)
				{

					e11 = alpha * h_d11i[i2][i1]; //fill matrices here with random numbers
					e12 = alpha * h_d12i[i2][i1]; //fill D matrix with random numbers
					e22 = alpha * h_d22i[i2][i1];
					r00 = h_ri[i2  ][i1  ];
					r01 = h_ri[i2  ][i1-1];
					r10 = h_ri[i2-1][i1  ];
					r11 = h_ri[i2-1][i1-1];
					rs = 0.25f*(r00+r01+r10+r11);
					ra = r00-r11;
					rb = r01-r10;
					r1 = ra-rb;
					r2 = ra+rb;
					s1 = e11*r1+e12*r2;
					s2 = e12*r1+e22*r2;
					sa = s1+s2;
					sb = s1-s2;
					si[i2  ][i1  ] += sa+rs;
					si[i2  ][i1-1] -= sb-rs;
					si[i2-1][i1  ] += sb+rs;
					si[i2-1][i1-1] -= sa-rs;
				}
			}
		
		
		
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
			
			err = CL.clEnqueueWriteBuffer(queue, d_r, CL.CL_FALSE, 0, Sizeof.cl_float*dims[0], phdata.to(h_ri1), 0, null, null);
			if(err != CL.CL_SUCCESS){
				System.out.println("Error: Could not write data to buffer");
				System.out.println("OpenCL Error Code: " + err);
				System.exit(1);
			}	
			
			err = CL.clEnqueueWriteBuffer(queue, d_d11, CL.CL_FALSE, 0, Sizeof.cl_float*dims[0], phdata.to(h_d11i_1), 0, null, null);
			if(err != CL.CL_SUCCESS){
				System.out.println("Error: Could not write data to buffer");
				System.out.println("OpenCL Error Code: " + err);
				System.exit(1);
			}	
			
			err = CL.clEnqueueWriteBuffer(queue, d_d12, CL.CL_FALSE, 0, Sizeof.cl_float*dims[0], phdata.to(h_d12i_1), 0, null, null);
			if(err != CL.CL_SUCCESS){
				System.out.println("Error: Could not write data to buffer");
				System.out.println("OpenCL Error Code: " + err);
				System.exit(1);
			}	
			
			err = CL.clEnqueueWriteBuffer(queue, d_d22, CL.CL_FALSE, 0, Sizeof.cl_float*dims[0], phdata.to(h_d22i_1), 0, null, null);
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
			
			
			err = CL.clEnqueueReadBuffer(queue, d_s, CL.CL_TRUE, 0, Sizeof.cl_float*(n1+1)*(n2+1), phdata.to(h_si1), 0, null, null);
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
					h_si[i][j] = h_si1[k];
					k++;
		
				}
			}
			
			
			//verify the results
			
			boolean check = true;
			int numfails = 0;
			for(int i = 0; i < n1; i++)
			{
				for(int j = 0; j < n2; j++)
				{
					if(Math.abs(si[i][j] - h_si[i][j]) < 0.001f) //changed here
					{
						continue;
					}
					else
					{
						check = false;
						System.out.printf("Check failed at array element (%d, %d)\n", i, j);
						System.out.printf("CPU Value: %f\n", si[i][j]);
						System.out.printf("GPU Value: %f\n", h_si[i][j]);
						++numfails;
					}
				}
			}
			
			
			if(check == true){
				System.out.println("Check Passed!");
				System.out.println();
			}
			else{
				System.out.println("Check Failed! Number of fails:" + numfails);
				System.out.println();
			}
			
			
			//finding the diffArray 
			float[][] diffArray;
			
			diffArray = ArrayMath.sub(si, h_si);
			
			
			//plot Resulting Images
			
			//PlotPanel panel = new PlotPanel(3,1);
			PlotPanel panel1 = new PlotPanel(1,1);
			PlotPanel panel2 = new PlotPanel(1,1);
			PlotPanel panel3 = new PlotPanel(1,1);
			
			PixelsView npix1 = panel1.addPixels(si);
			panel1.addTitle("CPU Output");
			panel1.addColorBar();
			PixelsView npix2 = panel2.addPixels(h_si);
			panel2.addTitle("GPU Output");
			panel2.addColorBar();
			PixelsView npix3 = panel3.addPixels(diffArray);
			panel3.addTitle("Difference Between Images");
			panel3.addColorBar();
	//		
	//		PixelsView pix1 = panel.addPixels(0,0,s);
	//		PixelsView pix2 = panel.addPixels(1,0,h_s);
	//		PixelsView pix3 = panel.addPixels(2,0,diffArray);
	//		panel.addColorBar();
			
			//PlotFrame frame = new PlotFrame(panel);
			PlotFrame frame1 = new PlotFrame(panel1);
			PlotFrame frame2 = new PlotFrame(panel2);
			PlotFrame frame3 = new PlotFrame(panel3);
			
			
			//frame.setVisible(true);
			frame1.setVisible(true);
			frame2.setVisible(true);
			frame3.setVisible(true);
			
			
	//		SimplePlot.asPixels(s).addColorBar();
	//		SimplePlot.asPixels(h_s).addColorBar();
	//		SimplePlot.asPixels(diffArray).addColorBar();
			
			//frame.paintToPng(50, 6, "test.png");
			frame1.paintToPng(50, 20, "cpuoutput.png");
			frame2.paintToPng(50, 20, "gpuoutput.png");
			frame3.paintToPng(50, 20, "imgdiff.png");
		
		}
		
		
		else if(programType == 2)
		{
			
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
			
			int maxTime = 5;
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
			
			//System.out.println("CPU count: " + cpucountf);
			//System.out.println("GPU count: " + gpucountf);
			System.out.println("CPU Benchmark: " + cpumflops + " MFLOPS");
			System.out.println("GPU Benchmark: " + gpumflops + " MFLOPS");
			
			
		}
		
		
		else if(programType == 3)
		{
			for(int size1 = 1; size1 < total; size1*=2)
			{
				System.out.println("Benchmark for Array Size: " + size1);
				System.out.println();
				
				int size2 = size1+2;// MUST be even
				int[] size1p = new int[1];
				size1p[0] = size1;
				int[] size2p = new int[1];
				size2p[0] = size2;
				int[] newdims =  new int[1];
				newdims[0] = size1 * size2;
				
				int x = 0;
				
				float[][] h_rp = new float[size1][size2];
				float[][] h_sp = new float[size1][size2];
				float[][] sp  = new float[size1][size2];
				float[][] h_d11p = new float[size1][size2];
				float[][] h_d12p = new float[size1][size2];
				float[][] h_d22p = new float[size1][size2];
				
				
				
				for(int i = 0; i < size1; i++)
				{
					for(int j = 0; j < size2; j++)
					{
						h_rp[i][j]   = rand.nextFloat()/RAND_MAX;
						h_d11p[i][j] = rand.nextFloat()/RAND_MAX;
						h_d12p[i][j] = rand.nextFloat()/RAND_MAX;
						h_d22p[i][j] = rand.nextFloat()/RAND_MAX;
						h_sp[i][j]   = 0.0f;
						sp[i][j]     = 0.0f;
						
					}
				}				
				
				//Packing Arrays
				
				float[] h_r1p = new float[newdims[0]];
				float[] h_s1p = new float[newdims[0]];
				float[] h_d11_1p = new float[newdims[0]];
				float[] h_d12_1p = new float[newdims[0]];
				float[] h_d22_1p = new float[newdims[0]];
				
				int k = 0;
				for(int i = 0; i < size1; i++)
				{
					for(int j = 0; j < size2; j++)
					{
						h_r1p[k]   = h_rp[i][j];
						h_s1p[k]   = h_sp[i][j];
						h_d11_1p[k] = h_d11p[i][j];
						h_d12_1p[k] = h_d12p[i][j];
						h_d22_1p[k] = h_d22p[i][j];
						k++;
					}
				}
				
				int maxTime = 5;
				int cpucount = 0;
				sw.restart();
				while(sw.time() < maxTime)
				{
					for(int i2 = 1; i2 < size1; ++i2)
					{
						for(int i1 = 1; i1 < size2; ++i1)
						{

							e11 = alpha * h_d11p[i2][i1]; //fill matrices here with random numbers
							e12 = alpha * h_d12p[i2][i1]; //fill D matrix with random numbers
							e22 = alpha * h_d22p[i2][i1];
							r00 = h_rp[i2  ][i1  ];
							r01 = h_rp[i2  ][i1-1];
							r10 = h_rp[i2-1][i1  ];
							r11 = h_rp[i2-1][i1-1];
							rs = 0.25f*(r00+r01+r10+r11);
							ra = r00-r11;
							rb = r01-r10;
							r1 = ra-rb;
							r2 = ra+rb;
							s1 = e11*r1+e12*r2;
							s2 = e12*r1+e22*r2;
							sa = s1+s2;
							sb = s1-s2;
							sp[i2  ][i1  ] += sa+rs;
							sp[i2  ][i1-1] -= sb-rs;
							sp[i2-1][i1  ] += sb+rs;
							sp[i2-1][i1-1] -= sa-rs;
						}
					}
					
					cpucount++;
					
				}
				sw.stop();
				float cputime = (float)sw.time();
//				
//				//OpenCL Benchmark
				
				cl_context contextp = CL.clCreateContext(null, 1, devices, null, null, errM);
				if(errM[0] != CL.CL_SUCCESS){
					System.out.println("Error: Failed to create the context.");
					System.out.println("OpenCL Error Code: " + err);
					System.exit(1);
				}
				

				
				cl_command_queue queuep = CL.clCreateCommandQueue(context, devices[0], 0, errM);
				if(errM[0] != CL.CL_SUCCESS){
					System.out.println("Error: Failed to create the command queue.");
					System.out.println("OpenCL error code: " + errM[0]);
					System.exit(1);
				}
				
				
				cl_mem d_sp, d_rp, d_d11p, d_d12p, d_d22p;
				
				//Allocating memory on the GPU
				
				d_rp = CL.clCreateBuffer(context, CL.CL_MEM_READ_ONLY, Sizeof.cl_float*(newdims[0]), null, errM);
				if(errM[0] != CL.CL_SUCCESS){
					System.out.println("Error: Could not create the buffer.");
					System.out.println("OpenCL Error Code: " + errM[0]);
					System.exit(1);
				}
				
				d_d11p = CL.clCreateBuffer(context, CL.CL_MEM_READ_ONLY, Sizeof.cl_float*(newdims[0]), null, errM);
				if(errM[0] != CL.CL_SUCCESS){
					System.out.println("Error: Could not create the buffer.");
					System.out.println("OpenCL Error Code: " + errM[0]);
					System.exit(1);
				}
				
				d_d12p = CL.clCreateBuffer(context, CL.CL_MEM_READ_ONLY, Sizeof.cl_float*(newdims[0]), null, errM);
				if(errM[0] != CL.CL_SUCCESS){
					System.out.println("Error: Could not create the buffer.");
					System.out.println("OpenCL Error Code: " + errM[0]);
					System.exit(1);
				}
				
				d_d22p = CL.clCreateBuffer(context, CL.CL_MEM_READ_ONLY, Sizeof.cl_float*(newdims[0]), null, errM);
				if(errM[0] != CL.CL_SUCCESS){
					System.out.println("Error: Could not create the buffer.");
					System.out.println("OpenCL Error Code: " + errM[0]);
					System.exit(1);
				}
				
				d_sp = CL.clCreateBuffer(context, CL.CL_MEM_READ_WRITE, Sizeof.cl_float*(size1+1)*(size2+1), null, errM);
				if(errM[0] != CL.CL_SUCCESS){
					System.out.println("Error: Could not create the buffer.");
					System.out.println("OpenCL Error Code: " + errM[0]);
					System.exit(1);
				}
				
//				//Copy the data to the GPU
//				

				long[] oned_global_work_size = new long[]{newdims[0]};
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
				
				err = CL.clSetKernelArg(kernel1, 0, Sizeof.cl_mem, phdata.to(d_rp));
				if(err != CL.CL_SUCCESS){
					System.out.println("Error: Could not set kernel argument.");
					System.out.println("OpenCL error code: " + err);
					System.exit(1);	
				}
				
				
				err = CL.clSetKernelArg(kernel1, 1, Sizeof.cl_mem, phdata.to(d_d11p));
				if(err != CL.CL_SUCCESS){
					System.out.println("Error: Could not set kernel argument.");
					System.out.println("OpenCL error code: " + err);
					System.exit(1);	
				}
				
				err = CL.clSetKernelArg(kernel1, 2, Sizeof.cl_mem, phdata.to(d_d12p));
				if(err != CL.CL_SUCCESS){
					System.out.println("Error: Could not set kernel argument.");
					System.out.println("OpenCL error code: " + err);
					System.exit(1);	
				}
				
				
				err = CL.clSetKernelArg(kernel1, 3, Sizeof.cl_mem, phdata.to(d_d22p));
				if(err != CL.CL_SUCCESS){
					System.out.println("Error: Could not set kernel argument.");
					System.out.println("OpenCL error code: " + err);
					System.exit(1);	
				}
				
				err = CL.clSetKernelArg(kernel1, 4, Sizeof.cl_mem, phdata.to(d_sp));
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
				
				err = CL.clSetKernelArg(kernel1, 6, Sizeof.cl_float, phdata.to(size1p));
				if(err != CL.CL_SUCCESS){
					System.out.println("Error: Could not set kernel argument.");
					System.out.println("OpenCL error code: " + err);
					System.exit(1);	
				}
				
				err = CL.clSetKernelArg(kernel1, 7, Sizeof.cl_float, phdata.to(size2p));
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
							int[] offsetxnp = new int[1];
							offsetxnp[0] = offsetx;
							//err = CL.clSetKernelArg(kernel1, 8, Sizeof.cl_float, phdata.to(offsetxnp));
							if(err != CL.CL_SUCCESS){
								System.out.println("Error: Could not set kernel argument.");
								System.out.println("OpenCL error code: " + err);
								System.exit(1);	
							}
							
							
//							int[] offsetyp = new int[1];
//							offsetyp[0] = offsety;
//							err = CL.clSetKernelArg(kernel1, 9, Sizeof.cl_float, phdata.to(offsetyp));
//							if(err != CL.CL_SUCCESS){
//								System.out.println("Error: Could not set kernel argument.");
//								System.out.println("OpenCL error code: " + err);
//								System.exit(1);	
//							}
//							
//							err = CL.clEnqueueNDRangeKernel(queue, kernel1, 2, null, global_group_size_block, local_group_size, 0, null, null);
//							if(err != CL.CL_SUCCESS){
//								System.out.println("Error: Could not execute the kernel");
//								System.out.println("OpenCL error code: " + err);
//								System.exit(1);
//							}
//							
						}
					}
//					
					gpucount++;
				}
//				
//				
				sw.stop();
				float gputime = (float)sw.time();
//				
//				err = CL.clEnqueueReadBuffer(queue, d_s, CL.CL_TRUE, 0, Sizeof.cl_float*(n1+1)*(n2+1), phdata.to(h_s1p), 0, null, null);
//				if(err != CL.CL_SUCCESS){
//					System.out.println("Error: Could not read the data from the kernel.");
//					System.out.println("OpenCL error code: " + err);
//					System.exit(1);
//				}
//				
//				CL.clReleaseMemObject(d_r);
//				CL.clReleaseMemObject(d_s);
//				CL.clReleaseMemObject(d_d11);
//				CL.clReleaseMemObject(d_d12);
//				CL.clReleaseMemObject(d_d22);
//				CL.clReleaseKernel(kernel0);
//				CL.clReleaseKernel(kernel1);
//				CL.clReleaseCommandQueue(queue);
//				CL.clReleaseProgram(program);
//				CL.clReleaseContext(context);
				
//				//unpacking the array
//				
//				k = 0;
//				for (int i = 0; i < n1; i++)
//				{
//					for (int j = 0; j < n2; j++)
//					{
//						h_sp[i][j] = h_s1p[k];
//						k++;
//			
//					}
//				}
//				
//				float cpucountf = (float)(cpucount);
//				float gpucountf = (float)(gpucount);
//				float numflops = 29.0f;
//				
//				
//				float cpumflops = (cpucountf * numflops * (size1-1) * (size2-1) * (float)1.0e-6)/cputime;
//				float gpumflops = (gpucountf * numflops * (size1-1) * (size2-1) * (float)1.0e-6)/gputime;
//				
//				System.out.println("CPU Benchmark: " + cpumflops + " MFLOPS");
//				System.out.println("GPU Benchmark: " + gpumflops + " MFLOPS");
//				System.out.println();
//				
//				cpudata[x] = cpumflops;
//				gpudata[x] = gpumflops;
//				x++;
//				
			}
		
			//create the plot here
			
			float[] xdata = new float[numPoints];
			
			for(int i = 0; i < numPoints; i++)
			{
				xdata[i] = (float)Math.pow(2.0d, (double)i);
			}
			

			PlotPanel panel = new PlotPanel(1,1);
			
			PointsView ptv0 = panel.addPoints(xdata, cpudata);
			ptv0.setStyle("r--");
			
			panel.addTitle("Performance of Smoothing Kernel");
			panel.setVLabel("MFLOPS");
			panel.setHLabel("Array Size");
			
			PlotFrame frame = new PlotFrame(panel);
			
			frame.setVisible(true);
			
			
			frame.paintToPng(50, 20, "arrayperfplot.png");
		
		}
		
			
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
