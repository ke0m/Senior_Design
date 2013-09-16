
public class benhmarkNorm {
	
	
	
public static void main(String args[]){
		
		int n1=2000;
		int n2=2001;
		
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
		
		double maxTime = 60;
		

		int counter3 = 0;
		

		arraySumJava norm = new arraySumJava();


		
		sw.start();
		do{			
			h_cc = norm.javaCPUArraySum(h_aa, h_bb, 500);
			System.out.println("SW Time:" + sw.time());
			//elapsedTime = elapsedTime + sw.time();
			counter3++;
		} while(sw.time() < maxTime);
		sw.stop();
		
		System.out.println("Number of MegaFLOPs: " + counter3*n1*n2/1000000);
}

}
