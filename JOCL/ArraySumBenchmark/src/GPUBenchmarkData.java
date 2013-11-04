import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintStream;


public class GPUBenchmarkData {
	
	
	public static void main(String args[]) throws FileNotFoundException{
		
<<<<<<< HEAD
		BenchmarkGPULoop data = new BenchmarkGPULoop();
		//BenchmarkCPULoop datacpu = new BenchmarkCPULoop();
		
		int minArraySize = 10000;
		int maxArraySize = 16000;
=======
		//BenchmarkGPULoop data = new BenchmarkGPULoop();
		//BenchmarkCPULoop datacpu = new BenchmarkCPULoop();
		BenchmarkOCLCPU dataoclcpu = new BenchmarkOCLCPU();


		int minArraySize = 500;
		int maxArraySize = 10000;
>>>>>>> 332558158123ba2d7a5b2daa118f13b6cc7b681c
		int minIters = 0;
		int maxIters = 1000;
		int k=0;
		int z=0;
		float[][] flopValues = new float[maxArraySize][maxIters];
		PrintStream output = new PrintStream(new File("data.txt"));
		
		for(int i = minArraySize; i <= maxArraySize; i+=100){
			System.out.println(i);
			k++;
			System.out.println(i);
			output.println();
			for(int j = minIters; j<=maxIters; j+=100){
<<<<<<< HEAD
				flopValues[k][z] = data.runBenchmark(j, i, i+1);
				//flopValues[k][z] = datacpu.runBenchmarkCPU(j, i, i+1);
=======
				//flopValues[k][z] = data.runBenchmark(j, i, i+1);
				//flopValues[k][z] = datacpu.runBenchmarkCPU(j, i, i+1);
				flopValues[k][z] = dataoclcpu.runBenchmarkOCLCPU(j, i, i+1);
>>>>>>> 332558158123ba2d7a5b2daa118f13b6cc7b681c
				output.print(flopValues[k][z] + " ");
				z++;
				
			}
		}		
	

	
	}

}

