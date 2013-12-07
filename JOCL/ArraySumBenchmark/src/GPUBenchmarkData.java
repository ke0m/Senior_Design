import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintStream;


public class GPUBenchmarkData {
	
	
	public static void main(String args[]) throws FileNotFoundException{
		
<<<<<<< HEAD
		//BenchmarkGPULoop data = new BenchmarkGPULoop();
		//BenchmarkCPULoop datacpu = new BenchmarkCPULoop();
		BenchmarkOCLCPU dataoclcpu = new BenchmarkOCLCPU();


		int minArraySize = 500;
		int maxArraySize = 10000;
		int minIters = 0;
		int maxIters = 1000;
=======
		BenchmarkGPULoop data = new BenchmarkGPULoop();
		//BenchmarkCPULoop datacpu = new BenchmarkCPULoop();
		//BenchmarkOCLCPU dataoclcpu = new BenchmarkOCLCPU();
		
		int minArraySize = 500;
		int maxArraySize = 10000;
		int minIters = 1;
		int maxIters = 100000;
>>>>>>> 17f84df76dd0d1428c04a63b074859006c5ff240
		int k=0;
		int z=0;
		int sizeStep = 100;
		int iterStep = 10;
		int row = (maxArraySize - minArraySize)/sizeStep;
		float[][] flopValues = new float[row][8];
		PrintStream output = new PrintStream(new File("data.txt"));
		
		for(int i = minArraySize; i <= maxArraySize; i+=100){
			k++;
			z = 0;
			System.out.println(i);
			output.println();
<<<<<<< HEAD
			for(int j = minIters; j<=maxIters; j+=100){
				//flopValues[k][z] = data.runBenchmark(j, i, i+1);
				//flopValues[k][z] = datacpu.runBenchmarkCPU(j, i, i+1);
				flopValues[k][z] = dataoclcpu.runBenchmarkOCLCPU(j, i, i+1);
=======
			for(int j = minIters; j<=maxIters; j*=10){
				flopValues[k][z] = data.runBenchmark(j, i, i+1);
				//flopValues[k][z] = datacpu.runBenchmarkCPU(j, i, i+1);
				//flopValues[k][z] = dataoclcpu.runBenchmarkOCLCPU(j, i, i+1);
>>>>>>> 17f84df76dd0d1428c04a63b074859006c5ff240
				output.print(flopValues[k][z] + " ");
				z++;
				System.out.println(z);
				
			}
		}		
	

	
	}

}

