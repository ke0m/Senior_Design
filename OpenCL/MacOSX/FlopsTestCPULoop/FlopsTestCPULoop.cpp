/*
 * File Name: FlopsTestCPULoop.cpp
 * Author: Joseph Jennings
 * Date: August 17, 2013
 * Description: This is normal C++ code that is used in order to compare the performance of the program 
 * 				"FlopsTestGPU.cpp". This comparison will allow us to see how many FLOPs are required in order
 * 				to programming the GPU efficient.
 */


//Include the I/O, stdlib and time header files. The time header file will allow us to use the function "gettimeofday"
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, const char * argv[])
{
	//Declaring the variables for the time function in order to measure CPU performance.
    struct timeval tim1, tim2;
	double time;
	//Calling this function stores the current time in seconds (GMT time starting from Jan 1, 1970)
    gettimeofday(&tim1, NULL);
    
    //This value determines the size of the nxn (square array)             
    int n = 1000;
    
	//Allocating the memory for the nxn arrays of floats.
    float **x = (float**)malloc(sizeof(float*)*n);
    float **y = (float**)malloc(sizeof(float*)*n);
    float **z = (float**)malloc(sizeof(float*)*n);
    

	//Initializing the arrays.
    for(int i = 0; i<n; i++){
        x[i] = (float*)malloc(sizeof(float)*n);
        y[i] = (float*)malloc(sizeof(float)*n);
        z[i] = (float*)malloc(sizeof(float)*n);
        
        for(int j = 0; j<n; j++){
            x[i][j] = i+j;
            y[i][j] = i+j;
            
        }
    }
    /*
	 * Here the sum of each element of the arrays x and y is computed and then stored in to the other array z. Mathematically
	 * written as z = x + y. As is seen here, the y array is repeated exactly 760 times. This value was determined via trial and
	 * error and comparing the speed of the sum of the arrays with this program versus that of the sum on the GPU from the 
	 * program "flopsTestGPU.cpp". 
	 */

    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){

            z[i][j] = x[i][j] + y[i][j];
			for(int k = 0; k < 250; k++){
				z[i][j] += y[i][j];
			}
        }
    }
 
 	//Calling again the function gettimeofday in order to measure the time after the above computation.
    gettimeofday(&tim2, NULL);
	//Finding the difference between the two measured times.
    time = tim2.tv_sec - tim1.tv_sec;
	//Displaying the difference in seconds.
    std::cout << time + (tim2.tv_usec - tim1.tv_usec)/1000000.00 << std::endl;
    
    return 0;
}

