__kernel void arraysum(__global const float* restrict d_xx, __global const float* restrict d_yy, __global float* restrict d_zz, int iters, int dims)
{
	if(get_global_id(0) >= dims){return;}
	 //restrict qualifiers avoid pointer aliasing http://en.wikipedia.org/wiki/Pointer_aliasing
	int i = get_global_id(0); //why of zero?  I think for just one dimension (the x dimension). For y it could by get_global_id(1)
	
	float t_d_zz = d_xx[i] + d_yy[i]; //using a sort of shared memory but I am not entirely sure which kind.

	for(int j = 0; j < iters; j++)
		t_d_zz += d_yy[i];

	d_zz[i] = t_d_zz; //copying back to global memory so the host can see it.

}