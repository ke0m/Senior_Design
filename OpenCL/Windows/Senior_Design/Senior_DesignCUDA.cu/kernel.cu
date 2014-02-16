
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include "Stopwatch.h"

#include <stdio.h>


__global__ void arraySum(const float *d_xx, float* d_yy, int dims, int iters)
{
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	if(x >= dims){return;}
	register float r_d_zz = d_xx[x] + d_yy[x];
	for(int i = 0; i < iters; i++)
		r_d_zz += d_yy[i];
	d_yy[x] = r_d_zz;
}

int main(void)
{
 
	 //Declaring Variables for for loops
    int minIters = 1;
    int maxIters = 4096;
    int minSize = 100;
    int maxSize = 1000;
    int a = 0;	
    int b = 0;

		//writing flops data to a file
	std::ofstream outputfile;
	outputfile.open("..\\flops_results_files\\cuda_result_biggeriters.txt");
	
	cudaError_t cudaStatus;

	Stopwatch sw;

	//Outer most loop: This loop should encompass all code and is used in order to loop over
    //different values of the iterations and array size to create the data for the plot.
	//for(int grpsize = 128; grpsize < 2048; grpsize*=2){
		for(int i = minSize; i < maxSize; i+=100){
			for(int j = minIters; j < maxIters; j*=2){

            
				//Initializing the three 2D arrays
            
				int n1 = i;
				int n2 = i+1;
				int iters = j;
				const long dims = n1*n2;
            
				float **h_xx = new float*[n1];
				float **h_yy = new float*[n1];
				float **h_zz = new float*[n1];
            
				for(int x = 0; x<n1; x++){
                
					h_xx[x] = new float [n2];
					h_yy[x] = new float [n2];
					h_zz[x] = new float [n2];
                
					//Initializing the arrays.
					for(int y = 0; y<n2; y++){

						h_xx[x][y] = x+y;
						h_yy[x][y] = x+y;
                    
					}
                
				}
            
				//Declaring the three 1D arrays
				float *h_xx1 = new float[dims];
				float *h_yy1 = new float[dims];
				float *h_zz1 = new float[dims];
                
                
				//packing arrays
				int k = 0;
				for (int x = 0; x < n1; x++){
					for (int y = 0; y < n2; y++){
						h_xx1[k] = h_xx[x][y];
						h_yy1[k] = h_yy[x][y];
						k++;
					}
                    
				}

				float *d_xx, *d_yy, *d_zz;

				cudaStatus = cudaMalloc((void**)&d_xx, sizeof(float) * dims);
				if(cudaStatus != cudaSuccess){
					std::cout << "cudaMalloc Failed!" << std::endl;
					system("pause");
					exit(1);
				}

				cudaStatus = cudaMalloc((void**)&d_yy, sizeof(float) * dims);
				if(cudaStatus != cudaSuccess){
					std::cout << "cudaMalloc Failed!" << std::endl;
					system("pause");
					exit(1);
				}

				cudaStatus = cudaMemcpy(d_xx, h_xx1, dims * sizeof(float), cudaMemcpyHostToDevice);
				if(cudaStatus != cudaSuccess){
					std::cout << "cudaMemCpy Failed" << std::endl;
					system("pause");
					exit(1);
				}

				cudaStatus = cudaMemcpy(d_yy, h_yy1, dims * sizeof(float), cudaMemcpyHostToDevice);
				if(cudaStatus != cudaSuccess){
					std::cout << "cudaMemCpy Failed" << std::endl;
					system("pause");
					exit(1);
				}

				int maxTime = 5;
				int count = 0;
				sw.restart();
				while(sw.getTime() < maxTime){

					//int threadsPerBlock = grpsize;
					int threadsPerBlock = 1024;
					//dim3 blockSize(threadsPerBlock);
					//dim3 gridSize((ceil(dims/threadsPerBlock) * threadsPerBlock));
					int blocks = (dims + threadsPerBlock)/threadsPerBlock;
					arraySum<<<blocks, threadsPerBlock>>>(d_xx, d_yy, dims, j);

					cudaStatus = cudaGetLastError();
					if(cudaStatus != cudaSuccess){
						std::cout << "arraySum kernel launch failed: " << cudaGetErrorString(cudaStatus) << std::endl;
						system("pause");
						exit(1);
					}
			
					count++;
			
				}


				sw.stop();

				cudaStatus = cudaDeviceSynchronize();
				if(cudaStatus != cudaSuccess){
					std::cout << "cudaStatus returned error code " << cudaStatus << " after launching arraySum kernel." << std::endl;
					system("pause");
					exit(1);
				}
			

				cudaStatus = cudaMemcpy(h_zz1, d_yy, dims*sizeof(float), cudaMemcpyDeviceToHost);
				if(cudaStatus != cudaSuccess){
					std::cout << "cudaMemcpy failed!" << std::endl;
					system("pause");
					exit(1);
				}


				cudaFree(d_xx);
				cudaFree(d_yy);



				//unpacking the 1D array
				k = 0;
				for (int x = 0; x < n1; x++){
					for (int y = 0; y < n2; y++){
						h_zz[x][y] = h_zz1[k];
						k++;
					}
				}
                
				delete [] h_xx1;
				delete [] h_yy1;
				delete [] h_zz1;
      


				for (int x = 0; x < n1; x++)
				{
					delete [] h_xx[x];
					delete [] h_yy[x];
					delete [] h_zz[x];
				}
            
				delete [] h_xx;
				delete [] h_yy;
				delete [] h_zz;
            

            
				float n1f = (float) n1;
				float n2f = (float) n2;
				float countf = (float) count;

				std::cout << "Iters: " << iters << std::endl;
				//outputfile << "Iters: " << iters << std::endl;
				std::cout << "n1: " << n1f << std::endl;
				//outputfile << "n1: " << n1f << std::endl;
				std::cout << "n2: " << n2f << std::endl;
				//outputfile << "n2: " << n2f << std::endl;
				//std::cout << "Threads per Block: " << grpsize << std::endl;
				//outputfile << "Threads per Block: " << grpsize << std::endl;
				std::cout << "count: " << countf << std::endl;
				//outputfile<< "count: " << countf << std::endl;
				std::cout << "Time: " << sw.getTime() << std::endl;
				//outputfile << "Time: " << sw.getTime() << std::endl;
            
				float mflops = n1f*n2f*countf*iters*1.0e-06/sw.getTime();
            
				//std::cout << "Number of MegaFLOPs: " << n1f*n2f*500*countf*1.0e-6 << std::endl;
				std::cout << mflops << " MegaFLOPS" << std::endl;
				//outputfile << mflops << " MegaFLOPS" << std::endl;
				std::cout << std::endl;
				//outputfile << std::endl;

				//outputfile << mflops << std::endl;


				b++;

	
			}
	
			a++;
			std::cout << std::endl;
			outputfile << std::endl;


		//}

	}

	outputfile.close();					
	
	return 0;

}