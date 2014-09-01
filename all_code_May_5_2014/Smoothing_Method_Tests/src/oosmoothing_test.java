import static org.jocl.CL.*;

import java.io.FileNotFoundException;
import java.util.Random;
import java.util.Scanner;

import org.jocl.*;

import edu.mines.jtk.dsp.LocalSmoothingFilter;
import edu.mines.jtk.mosaic.PixelsView;
import edu.mines.jtk.mosaic.PlotFrame;
import edu.mines.jtk.mosaic.PlotPanel;
import edu.mines.jtk.util.*;

public class oosmoothing_test {
	
	public static void main(String args[]) throws FileNotFoundException
	{
		
		Random rand = new Random();
		Scanner in = new Scanner(System.in);
		Pointer phdata = new Pointer();
		Stopwatch sw = new Stopwatch();
		
		int n1 = 2;
		int n2 = n1+2;// MUST be even
		int size = n1*n2;
		
		String fileName;
		String sourceStr;
		
		float e11, e12, e22;
		float r00, r01, r10, r11;
		float rs, ra, rb, sa, sb;
		float r1, r2, s1, s2;
		float alpha = 18.0f;
		
		String kernelNames[] = {"setMem", "soSmoothingNew"};
		
		sourceStr = CLUtil.readFile("sosmoothing.cl");
		
		CLUtil.clInit(sourceStr, kernelNames);
		
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
				h_ri[i][j]   = 1.0f;
				h_si[i][j]   = 1.0f;
				si[i][j]     = 1.0f;
				if(j >= 1 && i >= 1)
				{
					h_d11i[i][j] = 1.0f;
					h_d12i[i][j] = 0.0f;
					h_d22i[i][j] = 1.0f;
				}
				
			}
		}
		
		float[] h_ri1 = new float[size];
		float[] h_si1 = new float[size];
		float[] h_d11i_1 = new float[size];
		float[] h_d12i_1 = new float[size];
		float[] h_d22i_1 = new float[size];
		
		CLUtil.packArray(n1, n2, h_ri, h_ri1);
		CLUtil.packArray(n1, n2, h_si, h_si1);
		CLUtil.packArray(n1, n2, h_d11i, h_d11i_1);
		CLUtil.packArray(n1, n2, h_d12i, h_d12i_1);
		CLUtil.packArray(n1, n2, h_d22i, h_d22i_1);
		
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
				si[i2  ][i1  ] += sa;
				si[i2  ][i1-1] -= sb;
				si[i2-1][i1  ] += sb;
				si[i2-1][i1-1] -= sa;
			}
		}
	
		cl_mem d_s, d_r, d_d11, d_d12, d_d22;
		
		d_r = CLUtil.createGPUBuffer(size, "r");
		d_d11 = CLUtil.createGPUBuffer(size, "r");
		d_d12 = CLUtil.createGPUBuffer(size, "r");
		d_d22 = CLUtil.createGPUBuffer(size, "r");
		d_s = CLUtil.createGPUBuffer((n1+1)*(n2+1), "rw");
		
		CLUtil.copyToBuffer(h_ri1, d_r, size);
		CLUtil.copyToBuffer(h_d11i_1, d_d11, size);
		CLUtil.copyToBuffer(h_d12i_1, d_d12, size);
		CLUtil.copyToBuffer(h_d22i_1, d_d22, size);
		
		CLUtil.setKernelArg(CLUtil.kernels[0], d_s, 0);
		CLUtil.setKernelArg(CLUtil.kernels[0], 0.0f, 1);
		CLUtil.setKernelArg(CLUtil.kernels[0], size, 2);
		
		
		
		long[] oned_global_work_size = new long[]{size};
		CLUtil.executeKernel(CLUtil.kernels[0], 1, oned_global_work_size);
		
		long[] local_group_size = new long[]{32, 32};
		long[] mapped_n1 = new long[]{n2/2};
		long[] mapped_n2 = new long[]{n1/2};
		long[] global_group_size_block = new long[]{(long)Math.ceil(mapped_n1[0]/local_group_size[0] + 1)*local_group_size[0], (long)Math.ceil(mapped_n2[0]/local_group_size[0] + 1) * local_group_size[1]};
		
		
		CLUtil.setKernelArg(CLUtil.kernels[1], d_r, 0);
		CLUtil.setKernelArg(CLUtil.kernels[1], d_d11, 1);
		CLUtil.setKernelArg(CLUtil.kernels[1], d_d12, 2);
		CLUtil.setKernelArg(CLUtil.kernels[1], d_d22, 3);
		CLUtil.setKernelArg(CLUtil.kernels[1], d_s, 4);
		CLUtil.setKernelArg(CLUtil.kernels[1], alpha, 5);
		CLUtil.setKernelArg(CLUtil.kernels[1], n1, 6);
		CLUtil.setKernelArg(CLUtil.kernels[1], n2, 7);
		
		
		for(int offsetx = 0; offsetx < 2; ++offsetx)
		{
			for(int offsety = 0; offsety < 2; ++offsety)
			{
				CLUtil.setKernelArg(CLUtil.kernels[1], offsetx, 8);
				CLUtil.setKernelArg(CLUtil.kernels[1], offsety, 9);
				CLUtil.executeKernel(CLUtil.kernels[1], 2, global_group_size_block, local_group_size);
				
			}
		}
		
		
		CLUtil.readFromBuffer(d_s, h_si1, (n1+1)*(n2+1));
		
		float[] test = new float[n1*n2];
		
		LocalSmoothingFilter.saxpy1(n1, n2, -1.0f, h_si1, test);
		
		clReleaseMemObject(d_r);
		clReleaseMemObject(d_s);
		clReleaseMemObject(d_d11);
		clReleaseMemObject(d_d12);
		clReleaseMemObject(d_d22);
		CLUtil.clRelease();
		
		
		ArrayMath.dump(si);
		ArrayMath.dump(h_si);
		
		CLUtil.unPackArray(n1, n2, h_si1, h_si);
		
		float[][] diffArray;
		
		diffArray = ArrayMath.sub(si, h_si);
		
		PlotPanel panel1 = new PlotPanel(1,1);
		PlotPanel panel2 = new PlotPanel(1,1);
		PlotPanel panel3 = new PlotPanel(1,1);
		PlotPanel panel4 = new PlotPanel(1,1);
		
		PixelsView npix1 = panel1.addPixels(si);
		panel1.addTitle("CPU Output");
		panel1.addColorBar();
		PixelsView npix2 = panel2.addPixels(h_si);
		panel2.addTitle("GPU Output");
		panel2.addColorBar();
		PixelsView npix3 = panel3.addPixels(diffArray);
		npix3.setClips(-0.0001f, 0.0001f);
		panel3.addTitle("Difference Between Images");
		panel3.addColorBar();
		PixelsView npix4 = panel4.addPixels(h_ri);
		panel4.addTitle("Input Image");
		panel4.addColorBar();
//		
//		PixelsView pix1 = panel.addPixels(0,0,s);
//		PixelsView pix2 = panel.addPixels(1,0,h_s);
//		PixelsView pix3 = panel.addPixels(2,0,diffArray);
//		panel.addColorBar();
		
		//PlotFrame frame = new PlotFrame(panel);
		PlotFrame frame1 = new PlotFrame(panel1);
		PlotFrame frame2 = new PlotFrame(panel2);
		PlotFrame frame3 = new PlotFrame(panel3);
		PlotFrame frame4 = new PlotFrame(panel4);
		
		
		//frame.setVisible(true);
		frame1.setVisible(true);
		frame2.setVisible(true);
		frame3.setVisible(true);
		frame4.setVisible(true);
		
		
	}

}
