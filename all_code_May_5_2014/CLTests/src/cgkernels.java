import edu.mines.jtk.dsp.LocalSmoothingFilter;
import edu.mines.jtk.util.*;
import static edu.mines.jtk.dsp.LocalSmoothingFilter.*;

import org.jocl.*;

import java.io.FileNotFoundException;
import java.util.*;


//TODO: Benchmark the other methods. This will allow for me to see which one acts as the bottleneck.

public class cgkernels {
	
	public static void main(String[] args) throws FileNotFoundException
	{
		Random rand = new Random();
		Stopwatch sw = new Stopwatch();
		boolean check = true;
		
		int n1 = 1024;
		int n2 = 1024;
		int dims = n1*n2;
		
		int maxTime = 2;
		int gpucount = 0;
		int cpucount = 0;
		double gputime;
		double cputime;
			
		float[][] xc = new float[n1][n2];
		float[][] yc = new float[n1][n2];
		float[][] xg = new float[n1][n2];
		float[][] yg = new float[n1][n2];
		
		float[] x1 = new float[dims];
		float[] y1 = new float[dims];
		
		
		
		for(int i = 0; i < n1; ++i)
		{
			for(int j = 0; j < n2; ++j)
			{
				xc[i][j] = i+j;//rand.nextFloat();
				yc[i][j] = 0.0f;
				yg[i][j] = 0.0f;
			}

		}
		
		CLUtil.packArray(n1, n2, xc, x1);
		CLUtil.packArray(n1, n2, yg, y1);
		
		String sourceStr = CLUtil.readFile("cgkernels.cl");
		String[] kernelNames = {"clcopy", "clsaxpy", "clsxpay", "cldot"};
		
		CLUtil.clInit(sourceStr, kernelNames);
		
		
		//testing the scopy method
		cl_mem d_x = CLUtil.createGPUBuffer(dims, "r");
		cl_mem d_y = CLUtil.createGPUBuffer(dims, "rw");
		
		CLUtil.copyToBuffer(x1, d_x, dims);
		
		CLUtil.setKernelArg(CLUtil.kernels[0], n1, 0);
		CLUtil.setKernelArg(CLUtil.kernels[0], n2, 1);
		CLUtil.setKernelArg(CLUtil.kernels[0], d_x, 2);
		CLUtil.setKernelArg(CLUtil.kernels[0], d_y, 3);
		
		long[] global_work_size_oned = {dims};
		
		sw.restart();
		while(sw.time() < maxTime)
		{
			CLUtil.executeKernel(CLUtil.kernels[0], 1, global_work_size_oned);
			gpucount++;
		}
		sw.stop();
		gputime = sw.time();
		
		sw.restart();
		while(sw.time() < maxTime)
		{
			LocalSmoothingFilter.scopy(xc,yc);
			cpucount++;
		}
		sw.stop();
		cputime = sw.time();
		
		System.out.println("copy GPU: " + gpucount/gputime);
		System.out.println("copy CPU: " + cpucount/cputime);
		
		CLUtil.readFromBuffer(d_y, y1, dims);
		
		CLUtil.unPackArray(n1, n2, y1, yg);
		
		
		
		//LocalSmoothingFilter.scopy(xc, yc);
		
//		for(int i = 0; i < n1; ++i)
//		{
//			for(int j = 0; j < n2; ++j)
//			{
//				if(yc[i][j] == yg[i][j])
//				{
//					continue;
//				}
//				else
//				{
//					System.out.println("Copy Failed at element:" + i + "," + j);
//					System.out.println("CPU Output: " + yc[i][j]);
//					System.out.println("GPU Output: " + yg[i][j]);
//					check = false;
//				}
//			}
//		}
//		
//		
//		if(check == false)
//			System.out.println("Copy Check Failed!");
//		else
//			System.out.println("Copy Check Passed!");
		
		
		//testing the saxpy method
		check = true;
		
		ArrayMath.zero(y1);
		ArrayMath.zero(yc);
		ArrayMath.zero(yg);
		
		CLUtil.copyToBuffer(y1, d_y, dims);
		
		float a = -1.0f;
		
		CLUtil.setKernelArg(CLUtil.kernels[1], n1, 0);
		CLUtil.setKernelArg(CLUtil.kernels[1], n2, 1);
		CLUtil.setKernelArg(CLUtil.kernels[1], a, 2);
		CLUtil.setKernelArg(CLUtil.kernels[1], d_x, 3);
		CLUtil.setKernelArg(CLUtil.kernels[1], d_y, 4);
		
		gpucount = 0;
		sw.restart();
		while(sw.time() < maxTime)
		{
			CLUtil.executeKernel(CLUtil.kernels[1], 1, global_work_size_oned);
			gpucount++;
		}
		sw.stop();
		gputime = sw.time();

		CLUtil.readFromBuffer(d_y, y1, dims);
		
		CLUtil.unPackArray(n1, n2, y1, yg);
		
		cpucount = 0;
		sw.restart();
		while(sw.time() < maxTime)
		{
			LocalSmoothingFilter.saxpy(a, xc, yc);
			cpucount++;
		}
		sw.stop();
		cputime = sw.time();
		
		System.out.println("saxpy GPU: " + gpucount/gputime);
		System.out.println("saxpy CPU: " + cpucount/cputime);
		
		
//		for(int i = 0; i < n1; ++i)
//		{
//			for(int j = 0; j < n2; ++j)
//			{
//				if(yc[i][j] == yg[i][j])
//				{
//					continue;
//				}
//				else
//				{
//					System.out.println("Saxpy Check Failed at element:" + i + "," + j);
//					System.out.println("CPU Output: " + yc[i][j]);
//					System.out.println("GPU Output: " + yg[i][j]);
//					check = false;
//				}
//			}
//		}
		
//		if(check == false)
//			System.out.println("Saxpy Check Failed!");
//		else
//			System.out.println("Saxpy Check Passed!");
		
		
		//testing the sxpay method 
		check = true;
				
		ArrayMath.zero(y1);
		ArrayMath.zero(yc);
		ArrayMath.zero(yg);
		
		CLUtil.copyToBuffer(y1, d_y, dims);
		
		float alpha = -2.0f;
		
		CLUtil.setKernelArg(CLUtil.kernels[2], n1, 0);
		CLUtil.setKernelArg(CLUtil.kernels[2], n2, 1);
		CLUtil.setKernelArg(CLUtil.kernels[2], alpha, 2);
		CLUtil.setKernelArg(CLUtil.kernels[2], d_x, 3);
		CLUtil.setKernelArg(CLUtil.kernels[2], d_y, 4);
		
		gpucount = 0;
		sw.restart();
		while(sw.time() < maxTime)
		{
			CLUtil.executeKernel(CLUtil.kernels[2], 1, global_work_size_oned);
			gpucount++;
		}
		sw.stop();
		gputime = sw.time();
		
		CLUtil.readFromBuffer(d_y, y1, dims);
		
		CLUtil.unPackArray(n1, n2, y1, yg);
		
		
		cpucount = 0;
		sw.restart();
		while(sw.time() < maxTime)
		{
			LocalSmoothingFilter.sxpay(alpha, xc, yc);
			cpucount++;
		}
		sw.stop();
		cputime = sw.time();
		
		System.out.println("sxpay GPU: " + gpucount/gputime);
		System.out.println("sxpay CPU: " + cpucount/cputime);

		
		
//		for(int i = 0; i < n1; ++i)
//		{
//			for(int j = 0; j < n2; ++j)
//			{
//				if(yc[i][j] == yg[i][j])
//				{
//					continue;
//				}
//				else
//				{
//					System.out.println("Sxpay Check Failed at element:" + i + "," + j);
//					System.out.println("CPU Output: " + yc[i][j]);
//					System.out.println("GPU Output: " + yg[i][j]);
//					check = false;
//				}
//			}
//		}
//		
//		if(check == false)
//			System.out.println("Sxpay Check Failed!");
//		else
//			System.out.println("Sxpay Check Passed!");
		
		

		int work_group_size = 1024;
		int num_groups = dims/work_group_size/4;
		float[] delta = new float[num_groups];
		float fdelta = 0.0f;
		float cdelta = 0.0f;
		cl_mem d_delta = CLUtil.createGPUBuffer(num_groups, "rw");
		
		
		for(int i = 0; i < n1; i++)
		{
			for(int j = 0; j < n2; j++)
			{
				x1[i*n2 + j] = i + j;//1000.0f*rand.nextFloat();
				y1[i*n2 + j] = i + j;
			}

		}
		
		CLUtil.copyToBuffer(x1, d_x, dims);
		CLUtil.copyToBuffer(y1, d_y, dims);
		
		
		CLUtil.setKernelArg(CLUtil.kernels[3], n1, 0);
		CLUtil.setKernelArg(CLUtil.kernels[3], n2, 1);
		CLUtil.setKernelArg(CLUtil.kernels[3], d_x, 2);
		CLUtil.setKernelArg(CLUtil.kernels[3], d_y, 3);
		CLUtil.setKernelArg(CLUtil.kernels[3], d_delta, 4);
		CLUtil.setLocalKernelArg(CLUtil.kernels[3], work_group_size*4, 5);
		
		long[] local_work_size_vec = {1024};
		long[] global_work_size_vec = {((long)Math.ceil(dims/local_work_size_vec[0]/4)+1)*local_work_size_vec[0]}; 
		
		gpucount = 0;
		sw.restart();
		while(sw.time() < maxTime)
		{
			CLUtil.executeKernel(CLUtil.kernels[3], 1, global_work_size_vec, local_work_size_vec);
			CLUtil.readFromBuffer(d_delta, delta, num_groups);
			for(int i = 0; i < num_groups; i++)
				fdelta += delta[i];
			gpucount++;
		}

		sw.stop();
		gputime = sw.time();
		
		cpucount = 0;
		sw.restart();
		while(sw.time() < maxTime)
		{
			cdelta = LocalSmoothingFilter.sdot1(n1, n2, x1, y1);
			cpucount++;
		}
		
		sw.stop();
		cputime = sw.time();
		
		
		System.out.println("dot GPU: " + (double)(gpucount/gputime));
		System.out.println("dot CPU: " + (double)(cpucount/cputime));
		
//		CLUtil.executeKernel(CLUtil.kernels[3], 1, global_work_size_vec, local_work_size_vec);
//		CLUtil.readFromBuffer(d_delta, delta, num_groups);
//		fdelta = ArrayMath.sum(delta);
//		
//		cdelta = LocalSmoothingFilter.sdot1(n1, n2, x1, y1);
		
		//FLOPS = iters*(dims-1)
		
//		System.out.println("Delta GPU: " + fdelta);
//		System.out.println("Delta CPU: " + cdelta);
		
		CL.clReleaseMemObject(d_x);
		CL.clReleaseMemObject(d_y);
		CL.clReleaseMemObject(d_delta);
		CLUtil.clRelease();
				
	}

}
