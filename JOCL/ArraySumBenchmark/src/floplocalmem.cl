__kernel void arraysum(__global const float *d_aa, 
                        __local float *l_d_aa,
                        __global const float *d_bb,
                        __local float *l_d_bb, 
                        __global float *d_cc,
                        __local float *l_d_cc)
{

	//In this example, the global_id(1) is the number of rows and global_id(0) is the columns
	//So when the kernel is called, the local work group size needs to be the size of the 
	//number of columns

	int i = get_global_id(1)*get_global_size(0) + get_global_id(0); //Index of the row
	int j = get_local_id(0); 

	l_d_aa[get_local_id(0)] = d_aa[i];
	l_d_bb[get_local_id(0)] = d_bb[i];

	read_mem_fence(CLK_LOCAL_MEM_FENCE);

	l_d_cc[get_local_id(0)] = l_d_aa[get_local_id(0)] + l_d_bb[get_local_id(0)]; 

	for(int j = 0; j < 500; j++){
   		 l_d_cc[get_local_id(0)] += l_d_bb[j];
	}

	d_cc[i] = l_d_cc[get_local_id(0)]; //copy the thread only back to global
}


