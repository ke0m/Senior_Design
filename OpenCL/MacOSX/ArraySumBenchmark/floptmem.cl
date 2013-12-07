__kernel void arraysum(__global const float *d_xx, __global const float *d_yy, __global float *d_zz, int iters)
{

    int i = get_global_id(0);

	float t_d_zz = d_xx[i] + d_yy[i];



<<<<<<< HEAD
    for(int j = 0; j < iters; j++){
=======
    for(int j = 0; j <iters; j++){
>>>>>>> 17f84df76dd0d1428c04a63b074859006c5ff240
        t_d_zz += d_yy[i];
    }
    

	d_zz[i] = t_d_zz;
			
}
