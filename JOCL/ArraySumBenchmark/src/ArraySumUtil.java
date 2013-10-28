import org.jocl.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

/**
 * This class contains the functions required to implement the summation of two 2D arrays on the GPU,
 * on the CPU using OpenCL, and on the CPU using only Java.
 * @author Joseph Jennings
 * @version 2013.09.28
 *
 */

public class ArraySumUtil {
	
	/**
	 * Packs a 2D array of floats into a 1D array so it can be sent to the GPU.
	 * @param n1 the number of rows of the array
	 * @param n2 the number of columns of the array
	 * @param array2D the 2D array that will be packed into the 1D array
	 * @param array1D the packed array that will contain the data from the 2D array
	 */
	public void packArray(int n1, int n2, float[][] array2D, float[] array1D){
		
		int k = 0;
		
		for(int i = 0;  i < n1; i++){
			for(int j = 0; j < n2; j++){
				
				array1D[k] = array2D[i][j];
				k++;
			}
		}
		
	}
	
	/**
	 * Unpacks a 1D array into the specified 2D array when reading the data from the GPU.
	 * @param n1 the number of rows in the 2D array
	 * @param n2 the number of columns in the 2D array
	 * @param array1D the 1D array being read from the device
	 * @param array2D the 2D array to which the data will be unpacked
	 */
	public void unPackArray(int n1, int n2, float[] array1D, float[][] array2D){
		
		int k = 0;
		for (int i = 0; i < n1; i++){
			for (int j = 0; j < n2; j++){
	            array2D[i][j] = array1D[k];
	            k++;
			}
		}
		
	}

	/**
	 * Reads a text file into a String.
	 * @param file the text file to be read into the String
	 * @return the string containing the text from the text file
	 */
	public String readFile(String file) throws FileNotFoundException{

		String text = new Scanner( new File(file)).useDelimiter("\\A").next();
		
		return text;
		
	}

	/**
	 * Executes the summation of the two 2D arrays of floats on the GPU. The primary
	 * purpose of this method is to perform benchmarking of this sum on the GPU. This
	 * method performs this operation through the implementation of a loop on the GPU
	 * that computes z = x + y + y + y...
	 * @param array1 one of the 2D arrays of floats that will be summed
	 * @param array2 the other 2D array of floats that will be summed
	 * @param iters the number of desired iterations to repeat the sum on the GPU. 
	 * 		  This will increase the number of FLOPS and keep the number of copies
	 * 		  constant.
	 * @return the summed array unpacked and read from the GPU.
	 */
	public float[][] arraySumGPULoop(float[][] array1, float[][] array2, int iters) throws FileNotFoundException {
		
		String num = Integer.toString(iters);
		
		 String kernelCode = readFile("floptmem.cl");
		
		GPU sum = new GPU(kernelCode, "arraysum", true);
		    
	    cl_mem d_xx, d_yy, d_zz;
	    
	    int n1 = array1.length;
	    int n2 = array1[0].length;
	    int dims = n1*n2;
	    
	    float[][] array3 = new float[n1][n2];
	    
	    float[] pArray1 = new float[dims];
	    float[] pArray2 = new float[dims];
	    float[] pArray3 = new float[dims];
	    
	    d_xx = sum.createFloatBuffer(dims);
	    d_yy = sum.createFloatBuffer(dims);
	    d_zz = sum.createFloatBuffer(dims);
	    
	    packArray(n1, n2, array1, pArray1);
	    packArray(n1, n2, array2, pArray2);
	    
	    sum.writeToDevice(d_xx, pArray1, dims);
	    sum.writeToDevice(d_yy, pArray2, dims);
	    
	    sum.setKernelArg(d_xx, 0);
	    sum.setKernelArg(d_yy, 1);
	    sum.setKernelArg(d_zz, 2);
	    	    
	    sum.executeKernel(dims);
	    
	    sum.readFromDevice(d_zz, pArray3, dims);
	    
	    unPackArray(n1, n2, pArray3, array3);
	    
	    sum.freeDeviceMem(d_xx);
	    sum.freeDeviceMem(d_yy);
	    sum.freeDeviceMem(d_zz);
	    sum.freeKernel();
	    sum.freeCommandQueue();
	    sum.freeProgram();
	    sum.freeContext();
	    
		return array3;
		
	}

	/**
	 * Executes the summation of the two 2D arrays of floats on the GPU. The primary
	 * purpose of this method is to perform benchmarking of this sum on the GPU.
	 * This method differs from the method arraySumGPULoop as instead of computing
	 * the sum z= x + y + y + y... with a third for loop, each sum as been written out and
	 * is stored in the file "flopstest.cl".
	 * @param array1 one of the 2D arrays of floats that will be summed
	 * @param array2 the other 2D array of floats that will be summed
	 * @param iters the number of desired iterations to repeat the sum on the GPU. 
	 * 		  This will increase the number of FLOPS and keep the number of copies
	 * 		  constant.
	 * @return the summed array unpacked and read from the GPU.
	 */
	public float [][] arraySumGPU(float[][] array1, float[][] array2) throws FileNotFoundException{

		String kernelCode = readFile("flopstest.cl");

		GPU sum = new GPU(kernelCode, "arraysum", true);
		    
	    cl_mem d_xx, d_yy, d_zz;
	    
	    int n1 = array1.length;
	    int n2 = array1[0].length;
	    int dims = n1*n2;
	    
	    float[][] array3 = new float[n1][n2];
	    
	    float[] pArray1 = new float[dims];
	    float[] pArray2 = new float[dims];
	    float[] pArray3 = new float[dims];
	    
	    d_xx = sum.createFloatBuffer(dims);
	    d_yy = sum.createFloatBuffer(dims);
	    d_zz = sum.createFloatBuffer(dims);
	    
	    packArray(n1, n2, array1, pArray1);
	    packArray(n1, n2, array2, pArray2);
	    
	    sum.writeToDevice(d_xx, pArray1, dims);
	    sum.writeToDevice(d_yy, pArray2, dims);
	    
	    sum.setKernelArg(d_xx, 0);
	    sum.setKernelArg(d_yy, 1);
	    sum.setKernelArg(d_zz, 2);
	    	    
	    sum.executeKernel(dims);
	    
	    sum.readFromDevice(d_zz, pArray3, dims);
	    
	    unPackArray(n1, n2, pArray3, array3);
	    
	    sum.freeDeviceMem(d_xx);
	    sum.freeDeviceMem(d_yy);
	    sum.freeDeviceMem(d_zz);
	    sum.freeKernel();
	    sum.freeCommandQueue();
	    sum.freeProgram();
	    sum.freeContext();
	    
		return array3;

	}



	
	/**
	 * Executes the summation of the two 2D arrays of floats on the CPU using OpenCL. This method,
	 * as with the method arraySumGPU, is used for the performance benchmarking and for the comparison
	 * of the OpenCL on the GPU, OpenCL on the CPU, and straight Java code on the CPU.
	 * @param array1 one of the 2D arrays of floats that will be summed 
	 * @param array2 the other 2D array of floats that will be summed
	 * @param iters the number of desired iterations to repeat the sum on the CPU. 
	 * @return the summed array unpacked and read from the CPU.
	 */
	public float[][] arraySumOCLCPU(float[][] array1, float[][] array2, int iters){
		
		 String num = Integer.toString(iters);
		 String kernelCode = 
					"__kernel void " +
					"arraysum(__global const float *d_aa,"+
					"		  __global const float *d_bb,"+
					"         __global float *d_cc)" +
					"{"+
					"     int i = get_global_id(0);"+
					"     d_cc[i] = d_aa[i] + d_bb[i];"+
					"	  for(int j = 0; j<"+ num +"; j++){" +
					"			d_cc[i] += d_bb[i];	 "+
					"	   }					"+
					"}";
			
			
			GPU sum = new GPU(kernelCode, "arraysum", false);
			
		    cl_mem d_xx, d_yy, d_zz;
		    
		    int n1 = array1.length;
		    int n2 = array1[0].length;
		    int dims = n1*n2;
		    
		    float[][] array3 = new float[n1][n2];
		    
		    float[] pArray1 = new float[dims];
		    float[] pArray2 = new float[dims];
		    float[] pArray3 = new float[dims];
		    
		    d_xx = sum.createFloatBuffer(dims);
		    d_yy = sum.createFloatBuffer(dims);
		    d_zz = sum.createFloatBuffer(dims);
		    
		    packArray(n1, n2, array1, pArray1);
		    packArray(n1, n2, array2, pArray2);
		    
		    sum.writeToDevice(d_xx, pArray1, dims);
		    sum.writeToDevice(d_yy, pArray2, dims);
		    
		    sum.setKernelArg(d_xx, 0);
		    sum.setKernelArg(d_yy, 1);
		    sum.setKernelArg(d_zz, 2);
		    	    
		    sum.executeKernel(dims);
		    
		    sum.readFromDevice(d_zz, pArray3, dims);
		    
		    unPackArray(n1, n2, pArray3, array3);
		    
		    sum.freeDeviceMem(d_xx);
		    sum.freeDeviceMem(d_yy);
		    sum.freeDeviceMem(d_zz);
		    sum.freeKernel();
		    sum.freeCommandQueue();
		    sum.freeProgram();
		    sum.freeContext();
		    
			return array3;
			
		}


	public float[][] arraySumCPU(float[][] array1, float[][] array2){

		 	int n1 = array1.length;
		 	int n2 = array1[0].length;
		 
		 	float[][] array3 = new float[n1][n2];
		 
			for (int i = 0; i<n1; i++){
				for (int j=0; j<n2; j++){
					 array3[i][j] = array1[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j]
						 +array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j] + array2[i][j];

				}

			}

			return array3;

		}

	
	/**
	 * Executes the summation of two 2D arrays of floats on the CPU using Java. The primary use
	 * of this method is to perform performance benchmark tests  and compare the results with those
	 * of the same sum on the GPU
	 * @param array1 one of the 2D arrays of floats that will be summed
	 * @param array2 array2 the other 2D array of floats that will be summed
	 * @param iters the number of desired iterations to repeat the sum on the CPU.
	 * @return the summed array
	 */
	public float[][] arraySumCPULoop(float[][] array1, float[][] array2, int iters){
		
		 int n1 = array1.length;
		 int n2 = array1[0].length;
		 
		 float[][] array3 = new float[n1][n2];
		 
		 
		for (int i = 0; i<n1; i++){
			for (int j=0; j<n2; j++){
				array3[i][j] = array1[i][j] + array2[i][j];
			for (int k=0; k<iters; k++){
				array3[i][j] += array2[i][j];
				}
			
			}	
			
		}
	
		return array3;
	
	}

	
	
}





