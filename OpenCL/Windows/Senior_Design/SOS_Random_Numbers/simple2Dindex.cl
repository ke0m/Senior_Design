__kernel void indexing(__global restrict float *d_xx, int n1, int n2)
{
	if(get_global_id(0) >= n1 || get_global_id(1) >= n2){return;}

	int i = get_global_id(0);
	int j = get_global_id(1);

	d_xx[i*n2 + j] = i*j;

}