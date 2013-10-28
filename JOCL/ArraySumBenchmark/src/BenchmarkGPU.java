import java.io.FileNotFoundException;

/**
 * This class is used for performance benchmark testing of a summation of two 2D arrays
 * of floats on the GPU. Using Prof. Dave Hale's Stopwatch class, it runs for a specified 
 * amount of time and then displays the number of FLOPs calculated in the sum.
 * @author Joseph Jennings
 * @version 28.09.2013
 *
 */

public class BenchmarkGPU {

	public static void main(String args[]) throws FileNotFoundException{
		
		int n1=1000;
		int n2=1001;
		
		float[][] h_aa = new float[n1][n2];
		float[][] h_bb = new float[n1][n2];

		for(int i = 0; i<n1; i++){
			for(int j = 0; j<n2; j++){
				
				h_aa[i][j] = i + j;
				h_bb[i][j] = i + j;
				
			}
			
		}
		
		Stopwatch sw = new Stopwatch();
		double maxTime = 10;
		int counter = 0;
		ArraySumUtil oclgpu = new ArraySumUtil();
		

		sw.restart();		
		while(sw.time() < maxTime){
			oclgpu.arraySumGPU(h_aa, h_bb);
			System.out.println("SW Time:" + sw.time());
			counter++;
		}
		sw.stop();
		
		float n1f = (float) n1;
		float n2f = (float) n2;
		float counterf = (float) counter;
		System.out.println("Number of MegaFLOPs: " + counterf*n1f*n2f*250f*1.0e-6);
		System.out.println("FLOPS Rate: " + counterf*n1f*n2f*250f*1.0e-6/maxTime + " MFLOPS/s");	
	
	}
	
}
