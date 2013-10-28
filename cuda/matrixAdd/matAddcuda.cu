#include "stdio.h"
#include "cuda_runtime.h"
#include "matAddcuda.h"

/**
 * CUDA Kernel Device code
 * Computes the matrix addition of a and b into c. 
 */


__global__ void matrixAdd(float *a, float *b, float *c, int n2)
{
  int x = blockIdx.x;
  int y = blockIdx.y;
  int i = (n2*y) + x;
  c[i] = a[i] + b[i];
}


/**
 * Host main routine
 */
/*int main(void)
{
  // Allocate host vectors
  float h_a[ROW][COL];
  float h_b[ROW][COL];
  float h_c[ROW][COL];
  float *d_a;
  float *d_b;
  float *d_c;

  printf("Host memory allocated \n");

  // Allocate memory 
  cudaMalloc((void **) &d_a, ROW*COL*sizeof(float));
  cudaMalloc((void **) &d_b, ROW*COL*sizeof(float));
  cudaMalloc((void **) &d_c, ROW*COL*sizeof(float));

  printf("Device memory allocated \n");

  for (int i1 = 0; i1 < ROW; i1++) {
    for (int i2 = 0; i2 < COL; i2++ ) {
      //float den = 1/(i1+i2);
      h_a[i1][i2] = i1;
      h_b[i1][i2] = i2;
      h_c[i1][i2] = 0;
    }
  }

  cudaMemcpy(d_a,h_a,ROW*COL*sizeof(float),cudaMemcpyHostToDevice);
  cudaMemcpy(d_b,h_b,ROW*COL*sizeof(float),cudaMemcpyHostToDevice);

  printf("Host memory copied to device memory \n");
  
  int numBlocks =1;
  dim3 threadsPerBlock(COL,ROW);
  matrixAdd<<<threadsPerBlock,numBlocks>>>(d_a,d_b,d_c);

  cudaMemcpy(h_c,d_c,ROW*COL*sizeof(float),cudaMemcpyDeviceToHost);

  printf("Device memory copied to host memory \n");

  printf("Output matrix : \n");

  for (int i1 = 0; i1 < ROW; i1++) {
    for (int i2 = 0; i2 < COL; i2++) {
      printf("%f + %f = %f \n",h_a[i1][i2],h_b[i1][i2],h_c[i1][i2]);
    }
  }
  
  cudaFree(d_a);
  cudaFree(d_b);
  cudaFree(d_c);
  return 0;
}
*/
