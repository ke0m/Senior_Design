#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <CL/opencl.h>
#include <string.h>
#include <math.h>
#include "Stopwatch.h"

int main(int argc, const char * argv[]) {

  const int n1   = 100;
  const int n2   = n1+2;
  const int n3   = n2+2;
  const int dims = n1*n2;

  std::ofstream inputData;
  std::ofstream cpuOutput;

  float ***h_r   =  new float**[n1];
  float ***s     =  new float**[n1];
  float ***h_d11 =  new float**[n1];
  float ***h_d12 =  new float**[n1];
  float ***h_d13 =  new float**[n1];
  float ***h_d22 =  new float**[n1];
  float ***h_d23 =  new float**[n1];
  float ***h_d33 =  new float**[n1];

  for(int i=0; i<n1; i++) {
    h_r[i]   = new float*[n2];
    s[i]     = new float*[n2];
    h_d11[i] = new float*[n2];
    h_d12[i] = new float*[n2];
    h_d13[i] = new float*[n2];
    h_d22[i] = new float*[n2];
    h_d23[i] = new float*[n2];
    h_d33[i] = new float*[n2];
    for(int j=0; j<n2; j++) {
      for(int k=0; k<n3; k++) {
        h_r[j][k]   = new float[n3];
        s[j][k]     = new float[n3];
        h_d11[j][k] = new float[n3];
        h_d12[j][k] = new float[n3];
        h_d13[j][k] = new float[n3];
        h_d22[j][k] = new float[n3];
        h_d23[j][k] = new float[n3];
        h_d33[j][k] = new float[n3];
        /* Initializing the arrays */
        h_r[i][j][k]   = (float)rand()/(float)RAND_MAX;
        h_d11[i][j][k] = (float)rand()/(float)RAND_MAX;
        h_d12[i][j][k] = (float)rand()/(float)RAND_MAX;
        h_d13[i][j][k] = (float)rand()/(float)RAND_MAX;
        h_d22[i][j][k] = (float)rand()/(float)RAND_MAX;
        h_d23[i][j][k] = (float)rand()/(float)RAND_MAX;
        h_d33[i][j][k] = (float)rand()/(float)RAND_MAX;
        s[i][j][k]     = 0.0f;
        //printf("i=%d j=%d k=%d h_r=%f\n", i,j,k,h_r[i][j][k]);
      }
      //printf("\n");
    }
  }

  float e11, e12, e13, e22, e23, e33;
  float r000, r001, r010, r011, r100, r101, r110, r111;
  float ra, rb, rc, rd, rs;
  float sa, sb, sc, sd;
  float r1, r2, r3;
  float s1, s2, s3;

  float alpha = 10.0f;

  for (int i3=1; i3<n1; ++i3) {
    for (int i2=1; i2<n2; ++i2) {
      for (int i1=1; i1<n3; ++i1) {
        e11 = alpha*h_d11[i3][i2][i1]; // smoothing
        e12 = alpha*h_d12[i3][i2][i1]; // tensor
        e13 = alpha*h_d13[i3][i2][i1]; // coefficients
        e22 = alpha*h_d22[i3][i2][i1];
        e23 = alpha*h_d23[i3][i2][i1];
        e33 = alpha*h_d33[i3][i2][i1];
        r000 = h_r[i3 ][i2 ][i1 ];
        r001 = h_r[i3 ][i2 ][i1-1];
        r010 = h_r[i3 ][i2-1][i1 ];
        r011 = h_r[i3 ][i2-1][i1-1];
        r100 = h_r[i3-1][i2 ][i1 ];
        r101 = h_r[i3-1][i2 ][i1-1];
        r110 = h_r[i3-1][i2-1][i1 ];
        r111 = h_r[i3-1][i2-1][i1-1];
        rs = 0.25f*(r000+r001+r010+r011+
            r100+r101+r110+r111); // for B’B
        ra = r000-r111;
        rb = r001-r110;
        rc = r010-r101;
        rd = r100-r011;
        r1 = ra-rb+rc+rd; // three
        r2 = ra+rb-rc+rd; // components of
        r3 = ra+rb+rc-rd; // gradient of r
        s1 = e11*r1+e12*r2+e13*r3; // multiplied by
        s2 = e12*r1+e22*r2+e23*r3; // the smoothing
        s3 = e13*r1+e23*r2+e33*r3; // tensor
        sa = s1+s2+s3;
        sb = s1-s2+s3;
        sc = s1+s2-s3;
        sd = s1-s2-s3;
        s[i3  ][i2  ][i1  ] += sa+rs;
        s[i3  ][i2  ][i1-1] -= sd-rs;
        s[i3  ][i2-1][i1  ] += sb+rs;
        s[i3  ][i2-1][i1-1] -= sc-rs;
        s[i3-1][i2  ][i1  ] += sc+rs;
        s[i3-1][i2  ][i1-1] -= sb-rs;
        s[i3-1][i2-1][i1  ] += sd+rs;
        s[i3-1][i2-1][i1-1] -= sa-rs;
      }
    }
  }

  for(int i=0; i<n1; i++) {
    for(int j=0; j<n2; j++) {
      delete [] h_r[i][j]; 
      delete [] s[i][j]; 
      delete [] h_d11[i][j]; 
      delete [] h_d12[i][j]; 
      delete [] h_d13[i][j]; 
      delete [] h_d22[i][j]; 
      delete [] h_d23[i][j]; 
      delete [] h_d33[i][j]; 
    }
    delete [] h_r[i]; 
    delete [] s[i]; 
    delete [] h_d11[i]; 
    delete [] h_d12[i]; 
    delete [] h_d13[i]; 
    delete [] h_d22[i]; 
    delete [] h_d23[i]; 
    delete [] h_d33[i]; 
  }

  delete [] h_r; 
  delete [] s; 
  delete [] h_d11; 
  delete [] h_d12; 
  delete [] h_d13; 
  delete [] h_d22; 
  delete [] h_d23; 
  delete [] h_d33; 


  cpuOutput.close();
  inputData.close();


  return 0;

}
