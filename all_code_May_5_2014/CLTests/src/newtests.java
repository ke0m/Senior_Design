import edu.mines.jtk.dsp.LocalSmoothingFilter;
import edu.mines.jtk.util.*;
import static edu.mines.jtk.dsp.LocalSmoothingFilter.*;
import static edu.mines.jtk.util.ArrayMath.sum;
import static org.jocl.CL.clReleaseMemObject;

import org.jocl.*;

import java.io.FileNotFoundException;
import java.util.*;

public class newtests {
	
	public static void main(String[] args) throws FileNotFoundException{
		
		
//		 String sourceStr ="__kernel void cldot(int n1, int n2, __global float4* a_vec, __global float4* b_vec, __global float* output, __local float4* partial_dot) { " +"\n" +
//				 		   "" + "\n" +
//   						   "    int gid = get_global_id(0);" + "\n" +
//                           "    int lid = get_local_id(0);" + "\n" +
//                           "    int group_size = get_local_size(0);" + "\n" + 
//                           "    if(gid > n1*n2) return;" + "\n" +
//                           "" + "\n" +
//                           "    /* Place product of global values into local memory */" + "\n" +
//                           "    partial_dot[lid] = a_vec[gid] * b_vec[gid];" + "\n" +
//                           "    barrier(CLK_LOCAL_MEM_FENCE);" + "\n" +
//                           "" + "\n" +
//                           "    /* Repeatedly add values in local memory */" + "\n" +
//                           "    for(int i = group_size/2; i>0; i >>= 1) {" + "\n" +
//                           "       if(lid < i) {" + "\n" +
//                           "           partial_dot[lid] += partial_dot[lid + i];" + "\n" +
//                           "        }" + "\n" +
//                           "    barrier(CLK_LOCAL_MEM_FENCE);" + "\n" +
//                           "    }" + "\n" +
//                           "" + "\n" +
//                           "/* Transfer final result to global memory */" + "\n" +
//                           "   if(lid == 0) {" + "\n" +
//                           "       output[get_group_id(0)] = dot(partial_dot[0], (float4)(1.0f));" + "\n" +
//                           "   }" + "\n" +
//                           "}";
		
		
		String sourceStr =
				  "__kernel void clcopy(int n1, int n2, __global const float* restrict d_x, __global float* restrict d_y)" +"\n" +
				  "{" + "\n" +
				  "    int i  = get_global_id(0);" + "\n" +
				  "    if(i > n1*n2) return;" + "\n" +
				  "    d_y[i] = d_x[i];" + "\n" +
				  "}" + "\n" +
				  "" +"\n" +
			  	  "__kernel void soSmoothingNew(__global const float* restrict d_r," + "\n" +
			  	  "                                                   __global const float* restrict d_d11," + "\n" +
			  	  "                                                   __global const float* restrict d_d12," + "\n" +
			  	  "                                                   __global const float* restrict d_d22," + "\n" +
			  	  "                                                   __global float* restrict d_s," + "\n" +
			  	  "                                                   float alpha," + "\n" +
			  	  "                                                   int n1," + "\n" +
			  	  "                                                   int n2," + "\n" +
			  	  "                                                   int offsetx," + "\n" +
			  	  "                                                   int offsety)" + "\n" +
			  	  "{" + "\n" +
			  	  "     int g1 = get_global_id(0);" + "\n" +
			  	  "     int g0 = get_global_id(1);" + "\n" +
			  	  "" + "\n" +
			  	  "		int i1 = g1 * 2 + 1 + offsetx;" + "\n" +
			  	  "		int i2 = g0 * 2 + 1 + offsety;" + "\n" +
			  	  "" + "\n" +
			  	  "     if (i1 >= n2) return;" + "\n" +
			  	  "     if (i2 >= n1) return;" + "\n" +
			  	  "" + "\n" + 
			  	  "		float e11, e12, e22, r00, r01, r10, r11, rs, ra, rb, r1, r2, s_1, s_2, s_a, s_b;" + "\n" +
			  	  "" + "\n" + 
			  	  "		e11 = alpha * d_d11[i2*n2 + i1];" + "\n" +
			  	  "		e12 = alpha * d_d12[i2*n2 + i1];" + "\n" +
			  	  "		e22 = alpha * d_d22[i2*n2 + i1];" + "\n" +
			  	  "		r00 = d_r[i2*n2+i1];" + "\n" + 
			  	  "		r01 = d_r[i2*n2+(i1-1)];" + "\n" + 
			  	  "		r10 = d_r[(i2-1)*n2 + i1];" + "\n" +
			  	  "		r11 = d_r[(i2-1)*n2 + (i1-1)];" + "\n" +
			  	  "		rs = 0.25f*(r00+r01+r10+r11);" + "\n" +
			  	  "		ra = r00-r11;" + "\n" +
			  	  "		rb = r01-r10;" + "\n" +
			  	  "		r1 = ra-rb;" + "\n" +
			  	  "		r2 = ra+rb;" + "\n" +
			  	  "		s_1 = e11*r1+e12*r2;" + "\n" +
			  	  "		s_2 = e12*r1+e22*r2;" + "\n" +
			  	  "		s_a = s_1+s_2;" + "\n" +
			  	  "		s_b = s_1-s_2;" + "\n" +
			  	  "		d_s[i2*n2 + i1] += s_a;" + "\n" +
			  	  "		d_s[i2*n2 + (i1 -1)] -= s_b;" + "\n" +
			  	  "		d_s[(i2-1)*n2 + i1] += s_b;" + "\n" +
			  	  "		d_s[(i2-1)*n2 + (i1-1)] -= s_a;" + "\n" +
			  	  "" + "\n" +
			  	  "" + "\n" + 
			  	  "}" + "\n" +
			  	  "" + "\n" +
			  	  "__kernel void clsaxpy(int n1, int n2, float a, __global const float* restrict d_x, __global float* restrict d_y)" + "\n" +
			  	  "{" + "\n" +
			  	  "     int i = get_global_id(0);" + "\n" +
			  	  "     if(i > n1*n2) return;" + "\n" +
			  	  "     d_y[i] += a*d_x[i];" + "\n" +
			  	  "}" + "\n" +
			  	  "" + "\n" +
			  	  "__kernel void cldot(int n1, int n2, __global float4* a_vec, __global float4* b_vec, __global float* output, __local float4* partial_dot) { " +"\n" +
		 		  "" + "\n" +
				  "    int gid = get_global_id(0);" + "\n" +
	              "    int lid = get_local_id(0);" + "\n" +
	              "    int group_size = get_local_size(0);" + "\n" + 
	              "    if(gid > n1*n2) return;" + "\n" +
	              "" + "\n" +
	              "    /* Place product of global values into local memory */" + "\n" +
	              "    partial_dot[lid] = a_vec[gid] * b_vec[gid];" + "\n" +
	              "    barrier(CLK_LOCAL_MEM_FENCE);" + "\n" +
	              "" + "\n" +
	              "    /* Repeatedly add values in local memory */" + "\n" +
	              "    for(int i = group_size/2; i>0; i >>= 1) {" + "\n" +
	              "       if(lid < i) {" + "\n" +
	              "           partial_dot[lid] += partial_dot[lid + i];" + "\n" +
	              "        }" + "\n" +
	              "    barrier(CLK_LOCAL_MEM_FENCE);" + "\n" +
	              "    }" + "\n" +
	              "" + "\n" +
	              "/* Transfer final result to global memory */" + "\n" +
	              "   if(lid == 0) {" + "\n" +
	              "       output[get_group_id(0)] = dot(partial_dot[0], (float4)(1.0f));" + "\n" +
	              "   }" + "\n" +
	              "}" + "\n" +
			  	  "__kernel void clsxpay(int n1, int n2, float a, __global const float* restrict x, __global float* restrict y)" + "\n" +
			  	  "{" + "\n" +
			  	  "	int i = get_global_id(0);" + "\n" +
			  	  " if(i > n1*n2) return;" + "\n" +
			  	  " y[i] = a*y[i] + x[i];" + "\n" +
			  	  "}";

		
		 
		//String sourceStr = CLUtil.readFile("cgkernels.cl");
		
		int n1 = 3;
		int n2 = 3;
		
		float[][] x = new float[n1][n2];
		float[][] y = new float[n1][n2];
		float[][] yc = new float[n1][n2];
		float[][] yg = new float[n1][n2];
		
		
		for(int i = 0; i < n1; i++)
		{
			for(int j = 0; j < n2; j++)
			{
				x[i][j] = i+j;
			}
		}
		
		int size = n1*n2;
		int size_y = (n1+1)*(n2+1);
		int num_groups = size/1024/4; //I need to change this so that it works for all GPUS
		float[] x1g = new float[size];
		float[] y1g = new float[size_y];
		float[] x1c = new float[size];
		float[] y1c = new float[size_y];
		float[] s1 = new float[size];
		float[] d11 = new float[size];
		float[] d12 = new float[size];
		float[] d22 = new float[size];
		//String[] kernelNames = {"clcopy","clsaxpy","clsxpay","cldot"};
		String[] kernelNames = {"clcopy","soSmoothingNew","clsaxpy","cldot","clsxpay"};
		//String[] kernelNames = {"cldot"};
		CLUtil.clInit(sourceStr, kernelNames);
		CLUtil.packArray(n1, n2, x, x1g);  //where do I unpack
		CLUtil.packArray(n1, n2, x, x1c);
		scopy(x1g,y1g);
		scopy(x1c,y1c);
		scopy(x,y);
		cl_mem d_x = CLUtil.createGPUBuffer(size, "r");
		cl_mem d_d11 = CLUtil.createGPUBuffer(size, "r");
		cl_mem d_d12 = CLUtil.createGPUBuffer(size, "r");
		cl_mem d_d22 = CLUtil.createGPUBuffer(size, "r");
		cl_mem d_y = CLUtil.createGPUBuffer(size_y, "rw");
		cl_mem d_d = CLUtil.createGPUBuffer(size, "rw");
		cl_mem d_r = CLUtil.createGPUBuffer(size, "rw");
		cl_mem d_q = CLUtil.createGPUBuffer(size, "rw");
		//cl_mem d_delta = CLUtil.createGPUBuffer(num_groups, "rw");
		CLUtil.copyToBuffer(d11, d_d11, size);
		CLUtil.copyToBuffer(d12, d_d12, size);
		CLUtil.copyToBuffer(d22, d_d22, size);
		CLUtil.copyToBuffer(x1g, d_x, size);
		CLUtil.copyToBuffer(y1g, d_y, size_y);
		float[] p_delta = new float[size/1024/4];
	    long[] local_work_size_vec = {1024};
	    long[] global_work_size_oned = {size};
	    long[] global_work_size_vec = {((long)Math.ceil(size/local_work_size_vec[0]/4)+1)*local_work_size_vec[0]};
//	    CLUtil.setKernelArg(CLUtil.kernels[3], n1, 0);
//	    CLUtil.setKernelArg(CLUtil.kernels[3], n2, 1);
//	    CLUtil.setKernelArg(CLUtil.kernels[3], d_x, 2);
//	    CLUtil.setKernelArg(CLUtil.kernels[3], d_x, 3);
//	    CLUtil.setKernelArg(CLUtil.kernels[3], d_delta, 4);
//	    CLUtil.setLocalKernelArg(CLUtil.kernels[3], 1024*4, 5);
//	    CLUtil.executeKernel(CLUtil.kernels[3], 1, global_work_size_vec, local_work_size_vec);
//	    CLUtil.readFromBuffer(d_delta, p_delta, size/1024/4);
//	    float delta = sum(p_delta);
//	    System.out.println("GPU: " + delta);
//	    delta = LocalSmoothingFilter.sdot(x, x);
//		System.out.println("CPU: " + delta);
		
		CLUtil.setKernelArg(CLUtil.kernels[4], n1, 0);
		CLUtil.setKernelArg(CLUtil.kernels[4], n2, 1);
		CLUtil.setKernelArg(CLUtil.kernels[4], 10.0f, 2);
		CLUtil.setKernelArg(CLUtil.kernels[4], d_x, 3);
		CLUtil.setKernelArg(CLUtil.kernels[4], d_y, 4);
		CLUtil.executeKernel(CLUtil.kernels[4], 1, global_work_size_oned);
		CLUtil.readFromBuffer(d_y, y1g, size_y);
		
		LocalSmoothingFilter.sxpay1(n1, n2, 10.0f, x1c, y1c);
		LocalSmoothingFilter.sxpay(10.0f, x, y);
		
//		ArrayMath.dump(y1g);
//		ArrayMath.dump(y1c);
		CLUtil.unPackArray(n1, n2, y1g, yg);
		CLUtil.unPackArray(n1, n2, y1c, yc);
		ArrayMath.dump(yg);
		ArrayMath.dump(yc);
		ArrayMath.dump(y);
		
		
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
		
	    clReleaseMemObject(d_x);
		clReleaseMemObject(d_y);
		clReleaseMemObject(d_d11);
		clReleaseMemObject(d_d12);
		clReleaseMemObject(d_d22);
		CLUtil.clRelease();
		//CLUtil.unPackArray(n1, n2, y1, y);
			
	}
	
	

}
