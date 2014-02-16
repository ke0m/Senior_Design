__kernel void arraysum(__global const float16* restrict d_xx, __global float16* restrict d_yy, __global float16* restrict d_zz, int iters, int dims)
{
	if(get_global_id(0)*16 >= dims){return;}
	 //restrict qualifiers avoid pointer aliasing http://en.wikipedia.org/wiki/Pointer_aliasing
	int i = get_global_id(0); //why of zero?  I think for just one dimension (the x dimension). For y it could by get_global_id(1)
	
	float16 t_d_zz = fma(d_xx[i], d_xx[i], d_yy[i]);

	float16 t_d_yy = d_yy[i];

	for(int j = 0; j < iters; j++)
		t_d_zz = fma(t_d_yy, t_d_yy, t_d_zz);

	d_zz[i] = t_d_zz; //copying back to global memory so the host can see it.

}