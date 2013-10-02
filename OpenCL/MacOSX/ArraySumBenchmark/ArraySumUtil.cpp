//
//  ArraySumUtil.cpp
//  ArraySum1.1
//
//  Created by Joseph Jennings on 10/1/13.
//  Copyright (c) 2013 Joseph Jennings. All rights reserved.
//

#include "ArraySumUtil.h"


#include <OpenCL/OpenCL.h>
#include <stdlib.h>
#include "GPU.h"




void  ArraySumUtil::packArray(int n1, int n2, float ** array2d, float *array1d){
    
    int k = 0;
    for (int i = 0; i < n1; i++){
        for (int j = 0; j < n2; j++){
            
            array1d[k] = array2d[i][j];
            k++;
        }
        
    }

}


void ArraySumUtil::unPackArray(int n1, int n2, float* array1d, float** array2d){
    
    int k = 0;
    
    for (int i=0; i < n1; i++)
        array2d[i] = (float*)malloc(sizeof(float)*n2);
    
    for (int i = 0; i < n1; i++){
        for (int j = 0; j < n2; j++){
            array2d[i][j] = array1d[k];
            k++;
        }
    }
    
}



float **ArraySumUtil::arraySumGPU(float** array1, float** array2, int n1, int n2){
    
    GPU test("flopstestloop.cl", "arraysum", true);

    int dims = n1*n2;
    
    float** array3 = new float*[n1];
    for (int i = 0; i < n2; ++i)
        array3[i] = new float[n2];
    
    
    cl_mem d_xx, d_yy, d_zz;
    
    d_xx = test.createFloatBuffer(dims);
    d_yy = test.createFloatBuffer(dims);
    d_zz = test.createFloatBuffer(dims);
    

    
    float *h_xx1 = new float[dims];
    float *h_yy1 = new float[dims];
    float *h_zz1 = new float[dims];
    
    packArray(n1, n2, array1, h_xx1);
    packArray(n1, n2, array2, h_yy1);
    
    test.writeToDevice(d_xx, h_xx1, dims);
    test.writeToDevice(d_yy, h_yy1, dims);
    
    
    test.setKernelArg(d_xx, 0);
    test.setKernelArg(d_yy, 1);
    test.setKernelArg(d_zz, 2);
    
    test.executeKernel(dims);
    
    test.readFromDevice(d_zz, h_zz1, dims);
    
    
    unPackArray(n1, n2, h_zz1, array3);
    
    
    test.freeDeviceMem(d_xx);
    test.freeDeviceMem(d_yy);
    test.freeDeviceMem(d_zz);
    test.freeKernel();
    //test.freeCommandQueue();
    test.freeProgram();
    test.freeContext();

    
    

    
    
    return array3;
    
}

float **ArraySumUtil::arraySumOCLCPU(float** array1, float** array2, int n1, int n2){
    
    GPU test("flopstestloop.cl", "arraysum", false);
    
    int dims = n1*n2;
    
    float** array3 = new float*[n1];
    for (int i = 0; i < n2; ++i)
        array3[i] = new float[n2];
    
    
    cl_mem d_xx, d_yy, d_zz;
    
    d_xx = test.createFloatBuffer(dims);
    d_yy = test.createFloatBuffer(dims);
    d_zz = test.createFloatBuffer(dims);
    
    
    
    float *h_xx1 = new float[dims];
    float *h_yy1 = new float[dims];
    float *h_zz1 = new float[dims];
    
    packArray(n1, n2, array1, h_xx1);
    packArray(n1, n2, array2, h_yy1);
    
    test.writeToDevice(d_xx, h_xx1, dims);
    test.writeToDevice(d_yy, h_yy1, dims);
    
    
    test.setKernelArg(d_xx, 0);
    test.setKernelArg(d_yy, 1);
    test.setKernelArg(d_zz, 2);
    
    test.executeKernel(dims);
    
    test.readFromDevice(d_zz, h_zz1, dims);
    
    
    unPackArray(n1, n2, h_zz1, array3);
    
    
    test.freeDeviceMem(d_xx);
    test.freeDeviceMem(d_yy);
    test.freeDeviceMem(d_zz);
    test.freeKernel();
    test.freeCommandQueue();
    test.freeProgram();
    test.freeContext();
    
    
    
    
    
    
    return array3;
    
}


float **ArraySumUtil::arraySumCPU(float** array1, float** array2, int n1, int n2){
    
    
    float **array3 = (float**)malloc(sizeof(float*)*n1);
    
    for(int i = 0; i<n1; i++)
        array3[i] = (float*)malloc(sizeof(float)*n2);
    
    
    for(int i = 0; i<n1; i++){
        for(int j = 0; j<n2; j++){
            
            array3[i][j] = array1[i][j] + array2[i][j];
			for(int k = 0; k < 250; k++){
				array3[i][j] += array2[i][j];
                
			}
        }
    }

    
    
    
    
    return array3;
}











