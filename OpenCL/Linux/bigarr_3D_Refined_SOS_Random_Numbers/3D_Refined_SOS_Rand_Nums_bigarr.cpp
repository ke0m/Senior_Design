#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <CL/opencl.h>
#include <string.h>
#include <math.h>
#include "Stopwatch.h"


/*
 * TODO-
 * 1. Give some overlap of two samples with each pass. This may help with edge effects.
 * 2. Convert the 3D kernel into a function
 * 3. Convert the data selction loops to a function
 */

int main(int argc, const char * argv[]) {

  const int n1   = 16;
  const int n2   = n1+2;
  const int n3   = n2+2;
  const int dims = n1*n2*n3;
  
  const int n1a  = n1/2;
  const int n2a  = n2/2;
  const int n3a  = n3/2;

  std::ofstream inputData;
  std::ofstream cpuOutput;
  std::ofstream cpu2Output;
  cpuOutput.open("cpuoutput.txt");
  cpu2Output.open("cpu2output.txt");
  inputData.open("inputData.txt");


  //Memory allocation and initliaziation
  float ***h_r   =  new float**[n1];
  float ***h_s   =  new float**[n1];
  float ***s     =  new float**[n1];
  float ***st    =  new float**[n1];
  float ***h_d11 =  new float**[n1];
  float ***h_d12 =  new float**[n1];
  float ***h_d13 =  new float**[n1];
  float ***h_d22 =  new float**[n1];
  float ***h_d23 =  new float**[n1];
  float ***h_d33 =  new float**[n1];

  for(int i=0; i<n1; i++) {
    h_r[i]   = new float*[n2];
    h_s[i]   = new float*[n2];
    s[i]     = new float*[n2];
    st[i]    = new float*[n2];
    h_d11[i] = new float*[n2];
    h_d12[i] = new float*[n2];
    h_d13[i] = new float*[n2];
    h_d22[i] = new float*[n2];
    h_d23[i] = new float*[n2];
    h_d33[i] = new float*[n2];
  }

  for(int i=0; i<n1; i++) {
    for(int j=0; j<n2; j++) {
      h_r[i][j]   = new float[n3];
      h_s[i][j]   = new float[n3];
      s[i][j]     = new float[n3];
      st[i][j]    = new float[n3];
      h_d11[i][j] = new float[n3];
      h_d12[i][j] = new float[n3];
      h_d13[i][j] = new float[n3];
      h_d22[i][j] = new float[n3];
      h_d23[i][j] = new float[n3];
      h_d33[i][j] = new float[n3];
    }
  }

  for(int i=0; i<n1; i++) {
    for(int j=0; j<n2; j++) {
      for(int k=0; k<n3; k++) {
        h_r[i][j][k]   = (float)rand()/(float)RAND_MAX;
        h_d11[i][j][k] = (float)rand()/(float)RAND_MAX;
        h_d12[i][j][k] = (float)rand()/(float)RAND_MAX;
        h_d13[i][j][k] = (float)rand()/(float)RAND_MAX;
        h_d22[i][j][k] = (float)rand()/(float)RAND_MAX;
        h_d23[i][j][k] = (float)rand()/(float)RAND_MAX;
        h_d33[i][j][k] = (float)rand()/(float)RAND_MAX;
        s[i][j][k]     = 0.0f;
        st[i][j][k]     = 0.0f;
      }
    }
  }

  //Halved arrays
  float ***h_ra   = new float**[n1a];
  float ***h_sa   = new float**[n1a];
  float ***s_a    = new float**[n1a];
  float ***h_d11a = new float**[n1a];
  float ***h_d12a = new float**[n1a];
  float ***h_d13a = new float**[n1a];
  float ***h_d22a = new float**[n1a];
  float ***h_d23a = new float**[n1a];
  float ***h_d33a = new float**[n1a];

  for(int i=0; i<n1a; i++) {
    h_ra[i]   = new float*[n2a];
    h_sa[i]   = new float*[n2a];
    s_a[i]     = new float*[n2a];
    h_d11a[i] = new float*[n2a];
    h_d12a[i] = new float*[n2a];
    h_d13a[i] = new float*[n2a];
    h_d22a[i] = new float*[n2a];
    h_d23a[i] = new float*[n2a];
    h_d33a[i] = new float*[n2a];
  }

  for(int i=0; i<n1a; i++) {
    for(int j=0; j<n2a; j++) {
      h_ra[i][j]   = new float[n3a];
      h_sa[i][j]   = new float[n3a];
      s_a[i][j]    = new float[n3a];
      h_d11a[i][j] = new float[n3a];
      h_d12a[i][j] = new float[n3a];
      h_d13a[i][j] = new float[n3a];
      h_d22a[i][j] = new float[n3a];
      h_d23a[i][j] = new float[n3a];
      h_d33a[i][j] = new float[n3a];
    }
  }

  //Copying the first pass to the fourthed arrays
  for(int i=0; i<n1a; i++) {
    for(int j=0; j<n2a; j++) {
      for(int k=0; k<n3a; k++) {
        h_ra[i][j][k]   = h_r[i][j][k];
        h_d11a[i][j][k] = h_d11[i][j][k];
        h_d12a[i][j][k] = h_d12[i][j][k];
        h_d13a[i][j][k] = h_d13[i][j][k];
        h_d22a[i][j][k] = h_d22[i][j][k];
        h_d23a[i][j][k] = h_d23[i][j][k];
        h_d33a[i][j][k] = h_d33[i][j][k];
        s_a[i][j][k]    = 0.0f;
      }
    }
  }


  float e11, e12, e13, e22, e23, e33;
  float r000, r001, r010, r011, r100, r101, r110, r111;
  float ra, rb, rc, rd, rs;
  float sa, sb, sc, sd;
  float r1, r2, r3;
  float s1, s2, s3;

  float alpha = 18.0f;

  //3D SOS kernel 
  // Full process
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

  //3D SOS kernel
  // First pass
  printf("Starting first pass!\n\n");
  for (int i3=1; i3<n1a; ++i3) {
    for (int i2=1; i2<n2a; ++i2) {
      for (int i1=1; i1<n3a; ++i1) {
        e11 = alpha*h_d11a[i3][i2][i1]; // smoothing
        e12 = alpha*h_d12a[i3][i2][i1]; // tensor
        e13 = alpha*h_d13a[i3][i2][i1]; // coefficients
        e22 = alpha*h_d22a[i3][i2][i1];
        e23 = alpha*h_d23a[i3][i2][i1];
        e33 = alpha*h_d33a[i3][i2][i1];
        r000 = h_ra[i3 ][i2 ][i1 ];
        r001 = h_ra[i3 ][i2 ][i1-1];
        r010 = h_ra[i3 ][i2-1][i1 ];
        r011 = h_ra[i3 ][i2-1][i1-1];
        r100 = h_ra[i3-1][i2 ][i1 ];
        r101 = h_ra[i3-1][i2 ][i1-1];
        r110 = h_ra[i3-1][i2-1][i1 ];
        r111 = h_ra[i3-1][i2-1][i1-1];
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
        s_a[i3  ][i2  ][i1  ] += sa+rs;
        s_a[i3  ][i2  ][i1-1] -= sd-rs;
        s_a[i3  ][i2-1][i1  ] += sb+rs;
        s_a[i3  ][i2-1][i1-1] -= sc-rs;
        s_a[i3-1][i2  ][i1  ] += sc+rs;
        s_a[i3-1][i2  ][i1-1] -= sb-rs;
        s_a[i3-1][i2-1][i1  ] += sd+rs;
        s_a[i3-1][i2-1][i1-1] -= sa-rs;
      }
    }
  }

  //Copy the remainder of the inputs
  //from the whole arrays to the 
  //halved arrays
  for(int i=n1a; i<n1; i++) {
    for(int j=0; j<n2a; j++) {
      for(int k=0; k<n3a; k++) {
        st[i-n1a][j][k]     = s_a[i-n1a][j][k]; //copying the results to the total array
//        printf("st: %f i: %d j:%d k:%d\n", st[i-n1a][j-n2a][k-n3a], i-n1a, j-n2a, k-n3a); 
        h_ra[i-n1a][j][k]   = h_r[i][j][k];
        h_d11a[i-n1a][j][k] = h_d11[i][j][k];
        h_d12a[i-n1a][j][k] = h_d12[i][j][k];
        h_d13a[i-n1a][j][k] = h_d13[i][j][k];
        h_d22a[i-n1a][j][k] = h_d22[i][j][k];
        h_d23a[i-n1a][j][k] = h_d23[i][j][k];
        h_d33a[i-n1a][j][k] = h_d33[i][j][k];
        s_a[i-n1a][j][k]    = 0.0f; //zeroing out the new output array
      }
    }
  }

  //3D SOS kernel
  //second pass
  //printf("Starting second pass!\n\n");
  for (int i3=1; i3<n1a; ++i3) {
    for (int i2=1; i2<n2a; ++i2) {
      for (int i1=1; i1<n3a; ++i1) {
        e11 = alpha*h_d11a[i3][i2][i1]; // smoothing
        e12 = alpha*h_d12a[i3][i2][i1]; // tensor
        e13 = alpha*h_d13a[i3][i2][i1]; // coefficients
        e22 = alpha*h_d22a[i3][i2][i1];
        e23 = alpha*h_d23a[i3][i2][i1];
        e33 = alpha*h_d33a[i3][i2][i1];
        r000 = h_ra[i3 ][i2 ][i1 ];
        r001 = h_ra[i3 ][i2 ][i1-1];
        r010 = h_ra[i3 ][i2-1][i1 ];
        r011 = h_ra[i3 ][i2-1][i1-1];
        r100 = h_ra[i3-1][i2 ][i1 ];
        r101 = h_ra[i3-1][i2 ][i1-1];
        r110 = h_ra[i3-1][i2-1][i1 ];
        r111 = h_ra[i3-1][i2-1][i1-1];
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
        s_a[i3  ][i2  ][i1  ] += sa+rs;
        s_a[i3  ][i2  ][i1-1] -= sd-rs;
        s_a[i3  ][i2-1][i1  ] += sb+rs;
        s_a[i3  ][i2-1][i1-1] -= sc-rs;
        s_a[i3-1][i2  ][i1  ] += sc+rs;
        s_a[i3-1][i2  ][i1-1] -= sb-rs;
        s_a[i3-1][i2-1][i1  ] += sd+rs;
        s_a[i3-1][i2-1][i1-1] -= sa-rs;
      }
    }
  }

  //Copy the remainder of the inputs
  //from the whole arrays to the 
  //halved arrays
  for(int i=0; i<n1a; i++) {
    for(int j=n2a; j<n2; j++) {
      for(int k=0; k<n3a; k++) {
        st[i+n1a][j-n2a][k] = s_a[i][j-n2a][k]; //copying the results to the total array
//        printf("st: %f i: %d j:%d k:%d\n", st[i-n1a][j-n2a][k-n3a], i-n1a, j-n2a, k-n3a); 
        h_ra[i][j-n2a][k]   = h_r[i][j][k];
        h_d11a[i][j-n2a][k] = h_d11[i][j][k];
        h_d12a[i][j-n2a][k] = h_d12[i][j][k];
        h_d13a[i][j-n2a][k] = h_d13[i][j][k];
        h_d22a[i][j-n2a][k] = h_d22[i][j][k];
        h_d23a[i][j-n2a][k] = h_d23[i][j][k];
        h_d33a[i][j-n2a][k] = h_d33[i][j][k];
        s_a[i][j-n2a][k]    = 0.0f; //zeroing out the new output array
      }
    }
  }

  //3D SOS kernel
  printf("Starting second pass!\n\n");
  for (int i3=1; i3<n1a; ++i3) {
    for (int i2=1; i2<n2a; ++i2) {
      for (int i1=1; i1<n3a; ++i1) {
        e11 = alpha*h_d11a[i3][i2][i1]; // smoothing
        e12 = alpha*h_d12a[i3][i2][i1]; // tensor
        e13 = alpha*h_d13a[i3][i2][i1]; // coefficients
        e22 = alpha*h_d22a[i3][i2][i1];
        e23 = alpha*h_d23a[i3][i2][i1];
        e33 = alpha*h_d33a[i3][i2][i1];
        r000 = h_ra[i3 ][i2 ][i1 ];
        r001 = h_ra[i3 ][i2 ][i1-1];
        r010 = h_ra[i3 ][i2-1][i1 ];
        r011 = h_ra[i3 ][i2-1][i1-1];
        r100 = h_ra[i3-1][i2 ][i1 ];
        r101 = h_ra[i3-1][i2 ][i1-1];
        r110 = h_ra[i3-1][i2-1][i1 ];
        r111 = h_ra[i3-1][i2-1][i1-1];
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
        s_a[i3  ][i2  ][i1  ] += sa+rs;
        s_a[i3  ][i2  ][i1-1] -= sd-rs;
        s_a[i3  ][i2-1][i1  ] += sb+rs;
        s_a[i3  ][i2-1][i1-1] -= sc-rs;
        s_a[i3-1][i2  ][i1  ] += sc+rs;
        s_a[i3-1][i2  ][i1-1] -= sb-rs;
        s_a[i3-1][i2-1][i1  ] += sd+rs;
        s_a[i3-1][i2-1][i1-1] -= sa-rs;
      }
    }
  }

  //Copy the remainder of the inputs
  //from the whole arrays to the 
  //halved arrays
  for(int i=n1a; i<n1; i++) {
    for(int j=n2a; j<n2; j++) {
      for(int k=0; k<n3a; k++) {
        st[i-n1a][j][k]     = s_a[i-n1a][j-n2a][k]; //copying the results to the total array
//        printf("st: %f i: %d j:%d k:%d\n", st[i-n1a][j-n2a][k-n3a], i-n1a, j-n2a, k-n3a); 
        h_ra[i-n1a][j-n2a][k]   = h_r[i][j][k];
        h_d11a[i-n1a][j-n2a][k] = h_d11[i][j][k];
        h_d12a[i-n1a][j-n2a][k] = h_d12[i][j][k];
        h_d13a[i-n1a][j-n2a][k] = h_d13[i][j][k];
        h_d22a[i-n1a][j-n2a][k] = h_d22[i][j][k];
        h_d23a[i-n1a][j-n2a][k] = h_d23[i][j][k];
        h_d33a[i-n1a][j-n2a][k] = h_d33[i][j][k];
        s_a[i-n1a][j-n2a][k]    = 0.0f; //zeroing out the new output array
      }
    }
  }

  //3D SOS kernel
  printf("Starting second pass!\n\n");
  for (int i3=1; i3<n1a; ++i3) {
    for (int i2=1; i2<n2a; ++i2) {
      for (int i1=1; i1<n3a; ++i1) {
        e11 = alpha*h_d11a[i3][i2][i1]; // smoothing
        e12 = alpha*h_d12a[i3][i2][i1]; // tensor
        e13 = alpha*h_d13a[i3][i2][i1]; // coefficients
        e22 = alpha*h_d22a[i3][i2][i1];
        e23 = alpha*h_d23a[i3][i2][i1];
        e33 = alpha*h_d33a[i3][i2][i1];
        r000 = h_ra[i3 ][i2 ][i1 ];
        r001 = h_ra[i3 ][i2 ][i1-1];
        r010 = h_ra[i3 ][i2-1][i1 ];
        r011 = h_ra[i3 ][i2-1][i1-1];
        r100 = h_ra[i3-1][i2 ][i1 ];
        r101 = h_ra[i3-1][i2 ][i1-1];
        r110 = h_ra[i3-1][i2-1][i1 ];
        r111 = h_ra[i3-1][i2-1][i1-1];
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
        s_a[i3  ][i2  ][i1  ] += sa+rs;
        s_a[i3  ][i2  ][i1-1] -= sd-rs;
        s_a[i3  ][i2-1][i1  ] += sb+rs;
        s_a[i3  ][i2-1][i1-1] -= sc-rs;
        s_a[i3-1][i2  ][i1  ] += sc+rs;
        s_a[i3-1][i2  ][i1-1] -= sb-rs;
        s_a[i3-1][i2-1][i1  ] += sd+rs;
        s_a[i3-1][i2-1][i1-1] -= sa-rs;
      }
    }
  }

  for(int i=n1a; i<n1; i++) {
    for(int j=n2a; j<n2; j++) {
      for(int k=0; k<n3a; k++) {
        st[i][j][k] = s_a[i-n1a][j-n2a][k]; //copying the results to the total array
        //printf("st: %f i: %d j: %d k: %d\n", st[i][j][k], i, j, k);
      }
    }
  }

  //Verify the results
  bool check = true;
  int numfails = 0;
  for(int i=0; i<n1; i++) {
    for(int j=0; j<n2; j++) {
      for(int k=0; k<n3; k++) {
        if(fabs(s[i][j][k] - st[i][j][k]) < 0.001f) {
          continue;
        }
        else {
          if(numfails < 100) {
            printf("Check failed at array element: (%d, %d, %d)\n", i, j, k);
            printf("CPU Value: %f\n", s[i][j][k]);
            printf("CPU2 Value: %f\n", st[i][j][k]);
          }
          else if(numfails == 100) {
            printf("Exceeded more than 100 failed computations!\n");
          }
          ++numfails;
          check = false;
        }
      }
    }
  }

  if(check == true) {
    printf("Check Passed!\n\n");
  }
  else {
    printf("Check Failed! Number of fails: %d\n\n", numfails);
  }


  //Writing out the computed values for plotting
  for(int i=0; i<n1; i++) {
    for(int j=0; j<n2; j++) {
      for(int k=0; k<n3; k++) {
        inputData << h_r[i][j][k] << " ";
        cpuOutput << s[i][j][k]   << " ";
        cpu2Output << st[i][j][k]   << " ";
      }
      inputData << std::endl;
      cpuOutput << std::endl;
      cpu2Output << std::endl;
    }
    inputData << std::endl << std::endl;
    cpuOutput << std::endl << std::endl;
    cpu2Output << std::endl << std::endl;
  }

  cpu2Output.close();
  cpuOutput.close();
  inputData.close();


  // Freeing 3D arrays
  for(int i=0; i<n1; i++) {
    for(int j=0; j<n2; j++) {
      delete [] h_r[i][j];
      delete [] h_s[i][j];
      delete [] s[i][j];
      delete [] st[i][j];
      delete [] h_d11[i][j];
      delete [] h_d12[i][j];
      delete [] h_d13[i][j];
      delete [] h_d22[i][j];
      delete [] h_d23[i][j];
      delete [] h_d33[i][j];
    }
    delete [] h_r[i];
    delete [] h_s[i];
    delete [] s[i];
    delete [] st[i];
    delete [] h_d11[i];
    delete [] h_d12[i];
    delete [] h_d13[i];
    delete [] h_d22[i];
    delete [] h_d23[i];
    delete [] h_d33[i];
  }

  delete [] h_r;
  delete [] h_s;
  delete [] s;
  delete [] st;
  delete [] h_d11;
  delete [] h_d12;
  delete [] h_d13;
  delete [] h_d22;
  delete [] h_d23;
  delete [] h_d33;

  // Freeing halved 3D arrays
  for(int i=0; i<n1a; i++) {
    for(int j=0; j<n2a; j++) {
      delete [] h_ra[i][j];
      delete [] h_sa[i][j];
      delete [] s_a[i][j];
      delete [] h_d11a[i][j];
      delete [] h_d12a[i][j];
      delete [] h_d13a[i][j];
      delete [] h_d22a[i][j];
      delete [] h_d23a[i][j];
      delete [] h_d33a[i][j];
    }
    delete [] h_ra[i];
    delete [] h_sa[i];
    delete [] s_a[i];
    delete [] h_d11a[i];
    delete [] h_d12a[i];
    delete [] h_d13a[i];
    delete [] h_d22a[i];
    delete [] h_d23a[i];
    delete [] h_d33a[i];
  }

  delete [] h_ra;
  delete [] h_sa;
  delete [] s_a;
  delete [] h_d11a;
  delete [] h_d12a;
  delete [] h_d13a;
  delete [] h_d22a;
  delete [] h_d23a;
  delete [] h_d33a;

  //Plotting the results with python mayavi
  system("python volume_slicer.py &");
  system("python volume_slicer_cpu.py &");
  system("python volume_slicer_cpu2.py &");
  system("python volume_slicer_diffcpu.py &");


  return 0;

}

