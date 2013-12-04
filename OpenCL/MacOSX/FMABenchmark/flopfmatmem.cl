__kernel void fusema(__global const float *d_xx, __global const float *d_yy, __global float *d_zz, int iters)
{

    int i = get_global_id(0);
    int a = 10;

    float t_d_zz = a*d_xx[i] + d_yy[i];



    for(int j = 0; j < iters; j++){
        t_d_zz += a*d_xx[i];
    }
    

    d_zz[i] = t_d_zz;
			
}
