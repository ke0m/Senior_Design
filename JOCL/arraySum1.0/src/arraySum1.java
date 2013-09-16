import org.jocl.*;




public class arraySum1 {

	
	
	public void packArray(int n1, int n2, float[][] array2D, float[] array1D){
		
		int k = 0;
		
		for(int i = 0;  i < n1; i++){
			for(int j = 0; j < n2; j++){
				
				array1D[k] = array2D[i][j];
				k++;
			}
		}
		
		
	}
	
	public void unPackArray(int n1, int n2, float[] array1D, float[][] array2D){
		
		int k = 0;
		for (int i = 0; i < n1; i++){
			for (int j = 0; j < n2; j++){
	            array2D[i][j] = array1D[k];
	            k++;
			}
		}
		
		
		
		
	}
	

	
	public float[][] arraySumGPU(float[][] array1, float[][] array2, int iters){
		
		String num = Integer.toString(iters);
		
		 String kernelCode = 
				"__kernel void " +
				"arraysum(__global const float *d_aa,"+
				"		  __global const float *d_bb,"+
				"         __global float *d_cc)" +
				"{"+
				"     int i = get_global_id(0);"+
				"     d_cc[i] = d_aa[i] + d_bb[i];"+
				"	  for(int j = 0; j<"+num+"; j++){" +
				"			d_cc[i] += d_bb[i];	 "+
				"	   }					"+
				"}";
		
		
		
		
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
	    
		return array3;
		
		
	}
	
	public static void main(String args[]){
	
		arraySum1 run = new arraySum1();
		
		int n1 = 2;
		int n2 = 3;
		
		float[][] h_aa = new float[n1][n2];
		float[][] h_bb = new float[n1][n2];
		float[][] h_cc = new float[n1][n2];
		
		
		for(int i = 0; i<n1; i++){
			for(int j = 0; j<n2; j++){
				
				h_aa[i][j] = i + j;
				h_bb[i][j] = i + j;
				
			}
			
		}
		
		h_cc = run.arraySumGPU(h_aa, h_bb, 250);
		
        for(int i = 0; i < n1; i++){
            for(int j = 0; j < n2; j++){
    
                System.out.print(h_cc[i][j]+" ");
            }   
            System.out.print("\n");
        }   
		
	}
	
	
	
	
}
