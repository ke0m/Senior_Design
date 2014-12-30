__kernel void setMem(__global float *buffer, float value, int numFloats)
{
   if(get_global_id(0) < numFloats)
       buffer[get_global_id(0)] = value;
}

__kernel void soSmoothingNew3d(__global const float* restrict d_r,
						  __global const float* restrict d_d11, 
						  __global const float* restrict d_d12, 
						  __global const float* restrict d_d13, 
						  __global const float* restrict d_d22, 
						  __global const float* restrict d_d23, 
						  __global const float* restrict d_d33, 
						  __global float* restrict d_s, 
						  float alpha, 
						  int n1, 
						  int n2,
              int n3,
              int offsetx,
              int offsety,
              int offsetz)
{
    int g1 = get_global_id(0);
    int g0 = get_global_id(1);
    int g2 = get_global_id(2);

	int i1 = g1 * 2 + 1 + offsetx;// ycoord
	int i2 = g0 * 2 + 1 + offsety;// xcoord
	int i3 = g2 * 2 + 1 + offsetz;// zcoord

    if (i1 >= n2) return;
    if (i2 >= n1) return;
    if (i3 >= n3) return;

  float e11, e12, e13, e22, e23, e33;
  float r000, r001, r010, r011, r100, r101, r110, r111;
  float ra, rb, rc, rd, rs; 
  float s_a, s_b, s_c, s_d; 
  float r1, r2, r3; 
  float s_1, s_2, s_3;

	e11  = alpha * d_d11[i2+i1*n1+i3*n1*n2]; //fill matrices here with random numbers
	e12  = alpha * d_d12[i2+i1*n1+i3*n1*n2]; //fill D matrix with random numbers
	e13  = alpha * d_d13[i2+i1*n1+i3*n1*n2];
  e22  = alpha * d_d22[i2+i1*n1+i3*n1*n2];
  e23  = alpha * d_d23[i2+i1*n1+i3*n1*n2];
  e33  = alpha * d_d33[i2+i1*n1+i3*n1*n2];
	r000 = d_r[i2+i1*n1+i3*n1*n2];
	r001 = d_r[i2+i1*n1+(i3-1)*n1*n2];
	r010 = d_r[i2+(i1-1)*n1+i3*n1*n2];
	r011 = d_r[i2+(i1-1)*n1+(i3-1)*n1*n2]; 
  r100 = d_r[(i2-1)+i1*n1+i3*n1*n2]; 
  r101 = d_r[(i2-1)+i1*n1+(i3-1)*n1*n2];
  r110 = d_r[(i2-1)+(i1-1)*n1+i3*n1*n2];
  r111 = d_r[(i2-1)+(i1-1)*n1+(i3-1)*n1*n2]; 
	rs = 0.25f*(r000+r001+r010+r011+r100+r101+r110+r111); //check this later for mistake
	ra = r000-r111;
	rb = r001-r110;
  rc = r010-r101;
  rd = r100-r011;
	r1 = ra-rb+rc+rd;
	r2 = ra+rb-rc+rd;
  r3 = ra+rb+rc-rd;
	s_1 = e11*r1+e12*r2+e13*r3;
	s_2 = e12*r1+e22*r2+e23*r3;
	s_3 = e13*r1+e23*r2+e33*r3;
	s_a = s_1+s_2+s_3;
	s_b = s_1-s_2+s_3;
  s_c = s_1+s_2-s_3;
  s_d = s_1-s_2-s_3;
	d_s[i2+i1*n1+i3*n1*n2]              += s_a+rs;
	d_s[i2+i1*n1+(i3-1)*n1*n2]          -= s_d-rs;
	d_s[i2+(i1-1)*n1+i3*n1*n2]          += s_b+rs;
	d_s[i2+(i1-1)*n1+(i3-1)*n1*n2]      -= s_c-rs;
  d_s[(i2-1)+i1*n1+i3*n1*n2]          += s_c+rs;
  d_s[(i2-1)+i1*n1+(i3-1)*n1*n2]      -= s_b-rs;
  d_s[(i2-1)+(i1-1)*n1+i3*n1*n2]      += s_d+rs;
  d_s[(i2-1)+(i1-1)*n1+(i3-1)*n1*n2]  -= s_a-rs;

}
