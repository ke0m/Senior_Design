//
//  ArraySumUtil.h
//  ArraySum1.1
//
//  Created by Joseph Jennings on 10/1/13.
//  Copyright (c) 2013 Joseph Jennings. All rights reserved.
//

#ifndef __ArraySum1_1__ArraySumUtil__
#define __ArraySum1_1__ArraySumUtil__

#include <iostream>
#include <vector>

class ArraySumUtil{
    
    
public:
    
    void packArray(int n1, int n2, float** array2d, float* array1d);
	void packVector(std::vector<std::vector<float> > &vec2d, std::vector<float> &vec1d);
    void unPackArray(int n1, int n2, float* array1d, float** array2d);
    void printArray2D(int numRows, int numCols, float** array);
	void printVec2D(std::vector<std::vector<float> > &vec2d);
    void unPackVector(std::vector<float> &vec1d, std::vector<std::vector<float> > &vec2d);
	float** arraySumGPU(float** array1, float** array2, int n1, int n2, int iters);
    float** arraySumOCLCPU(float** array1, float** array2, int n1, int n2, int iters);
    float** arraySumCPULoop(float** array1, float** array2, int n1, int n2, int iters);
	float** arraySumCPU(float** array1, float** array2, int n1, int n2);
};





#endif /* defined(__ArraySum1_1__ArraySumUtil__) */
