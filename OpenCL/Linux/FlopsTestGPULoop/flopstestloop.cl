__kernel void arraysum(__global const float *d_aa, __global const float *d_bb, __global float *d_cc)
{

    int i = get_global_id(0);

	d_cc[i] = d_aa[i] + d_bb[i];


    for(int j = 0; j < 250; j++){
        d_cc[i] += d_bb[i];
    }
    
			
}
