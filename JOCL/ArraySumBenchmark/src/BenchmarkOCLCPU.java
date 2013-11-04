/**
 * This class is used for performance benchmark testing of a summation of two 2D arrays
 * of floats on the CPU using OpenCL. Using Prof. Dave Hale's Stopwatch class, it runs for a specified 
 * amount of time and then displays the number of FLOPs calculated in the sum.
 * @author Joseph Jennings
 * @version 2013.09.
 *
 */

public class BenchmarkOCLCPU {

	public float runBenchmarkOCLCPU(int iters, int n1, int n2){

		float mflops = 0;
		float[][] h_aa = new float[n1][n2];
		float[][] h_bb = new float[n1][n2];
		float[][] h_cc = new float[n1][n2];

		for(int i = 0; i<n1; i++){
			for(int j = 0; j<n2; j++){
				
				h_aa[i][j] = i + j;
				h_bb[i][j] = i + j;
				
			}
			
		}
		
		Stopwatch sw = new Stopwatch();
		
		double maxTime = 5;
		int counter = 0;
		ArraySumUtil oclcpu = new ArraySumUtil();

		sw.start();
		while(sw.time() < maxTime){
			h_cc = oclcpu.arraySumOCLCPU(h_aa, h_bb, iters);
			System.out.println("SW Time:" + sw.time());
			counter++;
		}
		sw.stop();
		
		
		float n1f = (float) n1;
		float n2f = (float) n2;
		float counterf = (float) counter;
		if(iters == 0){
			mflops = (float) (counterf*n1f*n2f*1.0e-6/sw.time());
		}
		else{
			mflops = (float) (counterf*n1f*n2f*iters*1.0e-6/sw.time());
		}
		System.out.println("Number of MegaFLOPs: "+ counterf*n1f*n2f*500f*1.0e-6);
		System.out.println("FLOPS Rate: " + counterf*n1f*n2f*500f*1.0e-6/maxTime + " MFLOPS/s");

		return mflops;

	}
	
		

}
