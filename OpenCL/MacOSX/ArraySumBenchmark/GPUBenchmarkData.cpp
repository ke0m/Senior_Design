<<<<<<< HEAD


=======
//
//  GPUBenchmarkData.cpp
//  ArraySumBenchmark
//
//  Created by Joseph Jennings on 11/23/13.
//  Copyright (c) 2013 Joseph Jennings. All rights reserved.
//

#include <iostream>
#include "GPU.h"
#include "ArraySumUtil.h"
#include "Stopwatch.h"
#include <OpenCL/OpenCL.h>
#include "BenchmarkGPU.h"
#include "BenchmarkCPU.h"
#include "BenchmarkOCLCPU.h"


int main (int argc, const char * argv[]){
    
    BenchmarkGPU datagpu;
    BenchmarkCPU datacpu;
    BenchmarkOCLCPU dataoclcpu;
    
    int minIters = 1;
    int maxIters = 4096;
    int minSize = 500;
    int maxSize = 2500;
    int a = 0;
    int b = 0;
    
    float** data = new float*[maxSize/minSize];
    for (int i = 0; i < 11; ++i)
        data[i] = new float[11];
    
    for(int i = minSize; i < maxSize; i+=100){
        std::cout << "Array Size: " << i << std::endl;
        for(int j = minIters; j < maxIters; j*=2){
            std::cout << "Iterations: " << j << std::endl;
            data[a][b] = datagpu.runGPUBenchmark(j, i, i+1);
            //data[a][b] = datacpu.runCPUBenchmark(j, i, i+1);
            //data[a][b] = dataoclcpu.runBenchmarkOCLCPU(j, i, i+1);
            b++;
        }
        a++;
        std::cout << std::endl;
    }
    
    return 0;
    
    
}
>>>>>>> 17f84df76dd0d1428c04a63b074859006c5ff240
