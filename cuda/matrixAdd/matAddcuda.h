#include "stdio.h"
#include "cuda_runtime.h"
#define COL 5
#define ROW 10

__global__ void matrixAdd(float *a, float *b, float *c, int n2);

