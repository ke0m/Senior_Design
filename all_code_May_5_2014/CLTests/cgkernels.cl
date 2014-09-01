__kernel void clcopy(int n1, int n2, __global const float*  restrict x, __global float* restrict y)
{
	int i = get_global_id(0);
	if (i > n1*n2) return;
	y[i] = x[i];
}

__kernel void clsaxpy(int n1, int n2, float a, __global const float* restrict x, __global float* restrict y)
{
	int i = get_global_id(0);
	if(i > n1*n2) return;

	y[i] += a*x[i];
}

__kernel void clsxpay(int n1, int n2, float a, __global const float* restrict x, __global float* restrict y)
{
	int i = get_global_id(0);
	if(i > n1*n2) return;

	y[i] = a*y[i] + x[i];
}

__kernel void cldot(int n1, int n2, __global float4* a_vec, __global float4* b_vec, __global float* output, __local float4* partial_dot) {

   int gid = get_global_id(0);
   int lid = get_local_id(0);
   int group_size = get_local_size(0);
   if(gid > n1*n2) return;

   /* Place product of global values into local memory */
   partial_dot[lid] = a_vec[gid] * b_vec[gid];
   barrier(CLK_LOCAL_MEM_FENCE);

   /* Repeatedly add values in local memory */
   for(int i = group_size/2; i>0; i >>= 1) {
      if(lid < i) {
         partial_dot[lid] += partial_dot[lid + i];
      }
      barrier(CLK_LOCAL_MEM_FENCE);
   }

   /* Transfer final result to global memory */
   if(lid == 0) {
      output[get_group_id(0)] = dot(partial_dot[0], (float4)(1.0f));
   }
}
