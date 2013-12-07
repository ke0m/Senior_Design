#include <iostream>
#include "matAddcpp.h"
#include "matAddcuda.h"
#include "stdio.h"
#include "cuda_runtime.h"
#include "Stopwatch.h"

__global__ void matrixAdd(float *a, float *b, float *c);

int main()
{
  int n1=1000;
  int n2=1001;
  
  vfloat1 a1(n1,0);
  
  vfloat2 a2(n2,a1);
  vfloat2 b2(a2);
  vfloat2 c2(a2);

  Stopwatch sw;
  double maxtime=2.0;
  int ncount;
  printf("Adding Matrices \n");
  printf("CPU \n");
  sw.restart();

  for (ncount=0; sw.getTime()<maxtime; ++ncount){
    matAddcpp(a2,b2,c2);
  }

  sw.stop();
  printf("mflops = %f  ncount = %d  time = %f \n", \
          1.0e-6*n1*n2*ncount/sw.getTime(),ncount,sw.getTime());
  // Allocating memory for matrices
  float **h_a = (float**)malloc(sizeof(float*)*n1);
  float **h_b = (float**)malloc(sizeof(float*)*n1);
  float **h_c = (float**)malloc(sizeof(float*)*n1);
  float *d_a;
  float *d_b;
  float *d_c;
  

  // Allocating memory on the device for matrices
  cudaMalloc((void **) &d_a, n1*n2*sizeof(float));
  cudaMalloc((void **) &d_b, n1*n2*sizeof(float));
  cudaMalloc((void **) &d_c, n1*n2*sizeof(float));
  
  // Initializing matrices
  for (int i1 = 0; i1 < n1; i1++) {
    h_a[i1] = (float*)malloc(sizeof(float)*n2);
    h_b[i1] = (float*)malloc(sizeof(float)*n2);
    h_c[i1] = (float*)malloc(sizeof(float)*n2);

    for (int i2 = 0; i2 < n2; i2++ ) {
      h_a[i1][i2] = i1;
      h_b[i1][i2] = i2;
      h_c[i1][i2] = 0;
    }
  }

  cudaMemcpy(d_a,h_a,n1*n2*sizeof(float),cudaMemcpyHostToDevice);
  cudaMemcpy(d_b,h_b,n1*n2*sizeof(float),cudaMemcpyHostToDevice);

  int numBlocks =1;
  dim3 threadsPerBlock(n2,n1);
  int ncountCU;

  printf("GPU \n");

  sw.restart();

  for (ncountCU=0; sw.getTime()<maxtime; ++ncountCU){
    matrixAdd<<<threadsPerBlock,numBlocks>>>(d_a,d_b,d_c,n2);
  }

  sw.stop();

  printf("mflops = %f  ncount = %d  time = %f \n", \
          1.0e-6*n1*n2*ncountCU/sw.getTime(),ncountCU,sw.getTime());

  cudaMemcpy(h_c,d_c,ROW*COL*sizeof(float),cudaMemcpyDeviceToHost);

  /*for (int i1 = 0; i1 < n1; i1++) {
    delete h_a[i1];
    delete h_b[i1];
    delete h_c[i1];
  }

  delete[] h_a;
  delete[] h_b;
  delete[] h_c;*/
}
