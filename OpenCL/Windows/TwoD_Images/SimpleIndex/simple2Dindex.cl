__kernel void setMem(__global float *buffer, float value, int numFloats)
{
   if(get_global_id(0) < numFloats)
       buffer[get_global_id(0)] = value;
}


__kernel void indexing(__global float* restrict d_xx, int n1, int n2)
{
	if(get_global_id(0) >= n1 || get_global_id(1) >= n2){return;}

	int i = get_global_id(0);
	int j = get_global_id(1);

	if(i >= 1 && j >= 1)
	{
		d_xx[i*n2 + j] = i;
		d_xx[i*n2 + (j-1)] = i;
		d_xx[(i-1)*n2 + j]     = i;
		d_xx[(i-1)*n2 + (j-1)] = i;
		
	}
}