__kernel void arraysum(__global const float *d_xx, __global const float *d_yy, __global float *d_zz)
{

    int i = get_global_id(0);
    d_zz[i] = d_yy[i] + d_yy[i];
    
}
