__kernel void arraysum(__global const float *d_xx, __global const float *d_yy, __global float *d_zz)
{

    int i = get_global_id(0);

	d_zz[i] = d_xx[i] + d_yy[i];


<<<<<<< HEAD
    for(int j = 0; j < 100; j++){
=======
    for(int j = 0; j < 250; j++){
>>>>>>> 6570116ca1328ff7cb674b44fd55746b2e548302
        d_zz[i] += d_yy[i];
    }
    
			
}
