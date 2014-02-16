
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include "cuComplex.h"
#include "time.h"
#include "cuda_runtime.h"
#include <iostream>

__global__ void test(float loop, float *out)
{
    register float a=1.0f;
    register float b=1.0f;
    register float c=1.0f;
    register float d=1.0f;
    register float e=1.0f;
    register float f=1.0f;
    register float g=1.0f;
    register float h=1.0f;

    for (float x=0;x<loop;x++)
    {
        a+=x*loop;
        b+=x*loop;
        c+=x*loop;
        d+=x*loop;
        e+=x*loop;
        f+=x*loop;
        g+=x*loop;
        h+=x*loop;

        a+=x*loop;
        b+=x*loop;
        c+=x*loop;
        d+=x*loop;
        e+=x*loop;
        f+=x*loop;
        g+=x*loop;
        h+=x*loop;

        a+=x*loop;
        b+=x*loop;
        c+=x*loop;
        d+=x*loop;
        e+=x*loop;
        f+=x*loop;
        g+=x*loop;
        h+=x*loop;

        a+=x*loop;
        b+=x*loop;
        c+=x*loop;
        d+=x*loop;
        e+=x*loop;
        f+=x*loop;
        g+=x*loop;
        h+=x*loop;

        a+=x*loop;
        b+=x*loop;
        c+=x*loop;
        d+=x*loop;
        e+=x*loop;
        f+=x*loop;
        g+=x*loop;
        h+=x*loop;
    }
    if (out!=NULL) *out=a+b+c+d+e+f+g+h;
}

int main(int argc, char *argv[])
{
    float timestamp;
    cudaEvent_t event_start,event_stop;
    // Initialise
    cudaDeviceReset();
    cudaSetDevice(0);
    cudaThreadSetCacheConfig(cudaFuncCachePreferShared);
    // Allocate and generate buffers
    cudaEventCreate(&event_start);
    cudaEventCreate(&event_stop);
    cudaEventRecord(event_start, 0);
    dim3 threadsPerBlock;
    dim3 blocks;
    threadsPerBlock.x=32;
    threadsPerBlock.y=32;
    threadsPerBlock.z=1;
    blocks.x=1;
    blocks.y=1000;
    blocks.z=1;

    test<<<blocks,threadsPerBlock,0>>>(1000,NULL);

    cudaEventRecord(event_stop, 0);
    cudaEventSynchronize(event_stop);
    cudaEventElapsedTime(&timestamp, event_start, event_stop);
    printf("Calculated in %f\n", timestamp);
	double mflops = (1000*1024*30*80*(1.0e-6))/timestamp;
	std::cout << "MegaFlops: " << mflops << std::endl;
	system("pause");
}