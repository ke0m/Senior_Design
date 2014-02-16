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
 
	
	
	return 0;

}