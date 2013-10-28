/*
 * File Name: "FlopsTestCPULoop.java"
 * Author: Joseph Jennings
 * Date: August 31, 2013
 * Description: This code sums two 2D arrays and measures the time it takes for the program to execute on the CPU. Its main 
 * 				purpose is used to compare this measured time with the time it takes to do perform the same sum on the GPU using
 * 				JOCL.
 */ 

public class FlopsTestCPULoop {

	
	public static void main(String[] args){
		
		//Variable set to start recording time.
		long startTime = System.nanoTime();
		
		//Determines the size of the nxn matrix
		int n = 1000;
		
		//Declaring the arrays
		float xx[][] = new float[n][n];
		float yy[][] = new float[n][n];
		float zz[][] = new float[n][n];
		

		//Initializing the three arrays
		for (int i = 0; i<n; i++){
			for (int j=0; j<n; j++){
				xx[i][j] = i + j;
				yy[i][j] = i + j;
			}
		}

		//Computing the sum and storing the result into the array zz
		for (int i = 0; i<n; i++){
			for (int j=0; j<n; j++){
				zz[i][j] = xx[i][j] + yy[i][j];
				for (int k=0; k<200; k++){
					zz[i][j] += yy[i][j];
				}
				
			}
		}
		
		//Measuring the end time and storing the difference between the end and start time.
		long elapsedTime = System.nanoTime() - startTime;
		
		//Printing the output value of the execution time
		System.out.println(elapsedTime);
		
	}
	
	
}
