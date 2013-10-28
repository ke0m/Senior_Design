__kernel void arraysum(__global const float *d_xx, __global const float *d_yy, __global float *d_zz)
{

    int i = get_global_id(0);
	
	float p_d_xx[1000];
	float p_d_yy[1000];


	for (int k = 0; k < 1000; k++)
			 p_d_xx[k] = d_xx[i*1000+k];
	for (int k = 0; k < 1000; k++)
			 p_d_yy[k] = d_yy[i*1000 + k];

	float t_d_zz = p_d_xx[i] + p_d_yy[i];



    for(int j = 0; j <250; j++){
        t_d_zz += p_d_yy[j];
    }
    

	d_zz[i] = t_d_zz;
			
}
