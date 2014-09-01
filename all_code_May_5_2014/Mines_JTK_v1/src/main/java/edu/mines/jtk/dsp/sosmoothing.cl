__kernel void setMem(__global float *buffer, float value, int numFloats)
{
   if(get_global_id(0) < numFloats)
       buffer[get_global_id(0)] = value;
}

__kernel void soSmoothingNew(__global const float* restrict d_r,
						  __global const float* restrict d_1, 
						  __global float* restrict d_s, 
						  float alpha, 
						  int n1, 
						  int n2,
                          		          int offsetx,
                                                  int offsety)
{
    int g1 = get_global_id(0);
    int g0 = get_global_id(1);

	int i1 = g1 * 2 + 1 + offsetx;
	int i2 = g0 * 2 + 1 + offsety;

    if (i1 >= n2) return;
    if (i2 >= n1) return;

	float e11, e12, e22, r00, r01, r10, r11, rs, ra, rb, r1, r2, s_1, s_2, s_a, s_b;

	e11 = alpha * d_d11[i2*n2 + i1]; //fill matrices here with random numbers
	e12 = alpha * d_d12[i2*n2 + i1]; //fill D matrix with random numbers
	e22 = alpha * d_d22[i2*n2 + i1];
	r00 = d_r[i2*n2+i1];
	r01 = d_r[i2*n2+(i1-1)];
	r10 = d_r[(i2-1)*n2 + i1];
	r11 = d_r[(i2-1)*n2 + (i1-1)];
	rs = 0.25f*(r00+r01+r10+r11);
	ra = r00-r11;
	rb = r01-r10;
	r1 = ra-rb;
	r2 = ra+rb;
	s_1 = e11*r1+e12*r2;
	s_2 = e12*r1+e22*r2;
	s_a = s_1+s_2;
	s_b = s_1-s_2;
	d_s[i2*n2 + i1] += s_a+rs;
	d_s[i2*n2 + (i1 -1)] -= s_b-rs;
	d_s[(i2-1)*n2 + i1] += s_b+rs;
	d_s[(i2-1)*n2 + (i1-1)] -= s_a-rs;


}