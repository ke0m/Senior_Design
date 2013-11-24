//
//  main.cpp
//  ArraySum1.1
//
//  Created by Joseph Jennings on 10/1/13.
//  Copyright (c) 2013 Joseph Jennings. All rights reserved.
//

#include <iostream>
#include "GPU.h"
#include "ArraySumUtil.h"
#include "Stopwatch.h"
#include <OpenCL/OpenCL.h>
#include "BenchmarkGPU.h"

float BenchmarkGPU::runGPUBenchmark(int iters, int n1, int n2)
{
    ArraySumUtil hope;
    Stopwatch sw;
    
    float **h_xx = (float**)malloc(sizeof(float*)*n1);
    float **h_yy = (float**)malloc(sizeof(float*)*n1);
	float **h_zz = (float**)malloc(sizeof(float*)*n1);
    
    for(int i = 0; i<n1; i++){
        h_xx[i] = (float*)malloc(sizeof(float)*n2);
        h_yy[i] = (float*)malloc(sizeof(float)*n2);
		h_zz[i] = (float*)malloc(sizeof(float)*n2);
        
        //Initializing the arrays.
        for(int j = 0; j<n2; j++){
            
            h_xx[i][j] = i+j;
            h_yy[i][j] = i+j;
            
        }
        
    }
    
    int maxTime = 5;
    int count = 0;
    
    sw.start();
    while (sw.getTime() < maxTime){
        h_zz = hope.arraySumGPU(h_xx, h_yy, n1, n2, iters);
        count++;
    }
    sw.stop();
    
    float n1f = (float) n1;
    float n2f = (float) n2;
    float countf = (float) count;
    
	//std::cout << "Number of MegaFLOPs: " << n1f*n2f*250*countf*1.0e-06 << std::endl;
    
    float mflops = (n1f*n2f*iters*countf*1e-06)/sw.getTime();
    
    std::cout << mflops << " MegaFLOPS" << std::endl;
                    
    return mflops;
    
}


