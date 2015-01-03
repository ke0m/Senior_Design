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
 *  a. To do this, I should start each new chunk overlapping the previous chunks
 * 2. Convert the 3D kernel into a function
 * 3. Convert the data selction loops to a function
 */

void soscpukernel3d(float ***r, float ***d11, float ***d12, float ***d13, float ***d22, float ***d23, float ***d33, float alpha, float ***s, int n1, int n2, int n3) {

  float e11, e12, e13, e22, e23, e33;
  float r000, r001, r010, r011, r100, r101, r110, r111;
  float ra, rb, rc, rd, rs;
  float sa, sb, sc, sd;
  float r1, r2, r3;
  float s1, s2, s3;

  for (int i3=1; i3<n1; ++i3) {
    for (int i2=1; i2<n2; ++i2) {
      for (int i1=1; i1<n3; ++i1) {
        e11 = alpha*d11[i3][i2][i1]; // smoothing
        e12 = alpha*d12[i3][i2][i1]; // tensor
        e13 = alpha*d13[i3][i2][i1]; // coefficients
        e22 = alpha*d22[i3][i2][i1];
        e23 = alpha*d23[i3][i2][i1];
        e33 = alpha*d33[i3][i2][i1];
        r000 = r[i3 ][i2 ][i1 ];
        r001 = r[i3 ][i2 ][i1-1];
        r010 = r[i3 ][i2-1][i1 ];
        r011 = r[i3 ][i2-1][i1-1];
        r100 = r[i3-1][i2 ][i1 ];
        r101 = r[i3-1][i2 ][i1-1];
        r110 = r[i3-1][i2-1][i1 ];
        r111 = r[i3-1][i2-1][i1-1];
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
}

void data_carve_prep(float ***r, float ***ra, float ***d11, float ***d11a, float ***d12, float ***d12a, float ***d13, float ***d13a, float ***d22, float ***d22a, float ***d23, float ***d23a, float ***d33, float ***d33a, float ***s_a, int istart, int jstart, int kstart, int n1, int n2, int n3) {

  for(int i=istart; i<n1; i++) {
    for(int j=jstart; j<n2; j++) {
      for(int k=kstart; k<n3; k++) {
        ra[i][j][k]   = r[i][j][k];
        d11a[i][j][k] = d11[i][j][k];
        d12a[i][j][k] = d12[i][j][k];
        d13a[i][j][k] = d13[i][j][k];
        d22a[i][j][k] = d22[i][j][k];
        d23a[i][j][k] = d23[i][j][k];
        d33a[i][j][k] = d33[i][j][k];
        s_a[i][j][k]  = 0.0f;
      }
    }
  }

}

//I need to offset here. I think I can check if istart, jstart, or kstart are not 0
void data_carve_prep(float ***r, float ***ra, float ***d11, float ***d11a, float ***d12, float ***d12a, float ***d13, float ***d13a, float ***d22, float ***d22a, float ***d23, float ***d23a, float ***d33, float ***d33a, float ***s, float ***s_a, int istart, int jstart, int kstart, int n1, int n2, int n3, float ***st) {

  for(int i=istart; i<n1; i++) {
    for(int j=jstart; j<n2; j++) {
      for(int k=kstart; k<n3; k++) {
        if(istart != 0) {
          ra[i-istart][j][k]   = r[i][j][k];
          d11a[i-istart][j][k] = d11[i][j][k];
          d12a[i-istart][j][k] = d12[i][j][k];
          d13a[i-istart][j][k] = d13[i][j][k];
          d22a[i-istart][j][k] = d22[i][j][k];
          d23a[i-istart][j][k] = d23[i][j][k];
          d33a[i-istart][j][k] = d33[i][j][k];
          s_a[i-istart][j][k]  = 0.0f;
        }
        else if(jstart != 0) {
          ra[i][j-jstart][k]   = r[i][j][k];
          d11a[i][j-jstart][k] = d11[i][j][k];
          d12a[i][j-jstart][k] = d12[i][j][k];
          d13a[i][j-jstart][k] = d13[i][j][k];
          d22a[i][j-jstart][k] = d22[i][j][k];
          d23a[i][j-jstart][k] = d23[i][j][k];
          d33a[i][j-jstart][k] = d33[i][j][k];
          s_a[i][j-jstart][k]  = 0.0f;
        }
        else if(kstart != 0) {
          ra[i][j][k-kstart]   = r[i][j][k];
          d11a[i][j][k-kstart] = d11[i][j][k];
          d12a[i][j][k-kstart] = d12[i][j][k];
          d13a[i][j][k-kstart] = d13[i][j][k];
          d22a[i][j][k-kstart] = d22[i][j][k];
          d23a[i][j][k-kstart] = d23[i][j][k];
          d33a[i][j][k-kstart] = d33[i][j][k];
          s_a[i][j][k-kstart]  = 0.0f;
        }
      }
    }
  }
}

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

  //Chunk arrays
  //Make each array big enough to contain two
  //extra samples in each dimension
  int overlap = 2;
  float ***h_ra   = new float**[n1a+overlap];
  float ***h_sa   = new float**[n1a+overlap];
  float ***s_a    = new float**[n1a+overlap];
  float ***h_d11a = new float**[n1a+overlap];
  float ***h_d12a = new float**[n1a+overlap];
  float ***h_d13a = new float**[n1a+overlap];
  float ***h_d22a = new float**[n1a+overlap];
  float ***h_d23a = new float**[n1a+overlap];
  float ***h_d33a = new float**[n1a+overlap];

  for(int i=0; i<n1a+overlap; i++) {
    h_ra[i]   = new float*[n2a+overlap];
    h_sa[i]   = new float*[n2a+overlap];
    s_a[i]    = new float*[n2a+overlap];
    h_d11a[i] = new float*[n2a+overlap];
    h_d12a[i] = new float*[n2a+overlap];
    h_d13a[i] = new float*[n2a+overlap];
    h_d22a[i] = new float*[n2a+overlap];
    h_d23a[i] = new float*[n2a+overlap];
    h_d33a[i] = new float*[n2a+overlap];
  }

  for(int i=0; i<n1a+overlap; i++) {
    for(int j=0; j<n2a+overlap; j++) {
      h_ra[i][j]   = new float[n3a+overlap];
      h_sa[i][j]   = new float[n3a+overlap];
      s_a[i][j]    = new float[n3a+overlap];
      h_d11a[i][j] = new float[n3a+overlap];
      h_d12a[i][j] = new float[n3a+overlap];
      h_d13a[i][j] = new float[n3a+overlap];
      h_d22a[i][j] = new float[n3a+overlap];
      h_d23a[i][j] = new float[n3a+overlap];
      h_d33a[i][j] = new float[n3a+overlap];
    }
  }

  float alpha = 18.0f;

  //process entire dataset
  soscpukernel3d(h_r, h_d11, h_d12, h_d13, h_d22, h_d23, h_d33, alpha, s, n1, n2, n3);

  //Copying the first pass to the fourthed arrays
  for(int i=0; i<n1a+overlap; i++) {
    for(int j=0; j<n2a+overlap; j++) {
      for(int k=0; k<n3a+overlap; k++) {
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

  //chunk 0,0,0
  soscpukernel3d(h_ra, h_d11a, h_d12a, h_d13a, h_d22a, h_d23a, h_d33a, alpha, s_a, n1a+overlap, n2a+overlap, n3a+overlap);

  //Here I should introduce some overlap
  for(int i=n1a; i<n1; i++) {
    for(int j=0; j<n2a; j++) {
      for(int k=0; k<n3a; k++) {
        st[i-n1a][j][k]     = s_a[i-n1a][j][k]; //copying the results to the total array
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

  //chunk 1,0,0
  soscpukernel3d(h_ra, h_d11a, h_d12a, h_d13a, h_d22a, h_d23a, h_d33a, alpha, s_a, n1a, n2a, n3a);

  //Copy the remainder of the inputs
  //from the whole arrays to the 
  //halved arrays
  for(int i=0; i<n1a; i++) {
    for(int j=n2a; j<n2; j++) {
      for(int k=0; k<n3a; k++) {
        st[i+n1a][j-n2a][k] = s_a[i][j-n2a][k]; //copying the results to the total array
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

  //chunk 0,1,0
  soscpukernel3d(h_ra, h_d11a, h_d12a, h_d13a, h_d22a, h_d23a, h_d33a, alpha, s_a, n1a, n2a, n3a);

  //from the whole arrays to the 
  //halved arrays
  for(int i=n1a; i<n1; i++) {
    for(int j=n2a; j<n2; j++) {
      for(int k=0; k<n3a; k++) {
        st[i-n1a][j][k]     = s_a[i-n1a][j-n2a][k]; //copying the results to the total array
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

  //chunk 1,1,0
  soscpukernel3d(h_ra, h_d11a, h_d12a, h_d13a, h_d22a, h_d23a, h_d33a, alpha, s_a, n1a, n2a, n3a);
  
  //from the whole arrays to the 
  //halved arrays
  for(int i=0; i<n1a; i++) {
    for(int j=0; j<n2a; j++) {
      for(int k=n3a; k<n3; k++) {
        st[i+n1a][j+n2a][k-n3a] = s_a[i][j][k-n3a]; //copying the results to the total array
        h_ra[i][j][k-n3a]       = h_r[i][j][k];
        h_d11a[i][j][k-n3a]     = h_d11[i][j][k];
        h_d12a[i][j][k-n3a]     = h_d12[i][j][k];
        h_d13a[i][j][k-n3a]     = h_d13[i][j][k];
        h_d22a[i][j][k-n3a]     = h_d22[i][j][k];
        h_d23a[i][j][k-n3a]     = h_d23[i][j][k];
        h_d33a[i][j][k-n3a]     = h_d33[i][j][k];
        s_a[i][j][k-n3a]        = 0.0f; //zeroing out the new output array
      }
    }
  }

  //chunk 0,0,1
  soscpukernel3d(h_ra, h_d11a, h_d12a, h_d13a, h_d22a, h_d23a, h_d33a, alpha, s_a, n1a, n2a, n3a);

  //from the whole arrays to the 
  //halved arrays
  for(int i=n1a; i<n1; i++) {
    for(int j=0; j<n2a; j++) {
      for(int k=n3a; k<n3; k++) {
        st[i-n1a][j][k]         = s_a[i-n1a][j][k-n3a]; //copying the results to the total array
        h_ra[i-n1a][j][k-n3a]   = h_r[i][j][k];
        h_d11a[i-n1a][j][k-n3a] = h_d11[i][j][k];
        h_d12a[i-n1a][j][k-n3a] = h_d12[i][j][k];
        h_d13a[i-n1a][j][k-n3a] = h_d13[i][j][k];
        h_d22a[i-n1a][j][k-n3a] = h_d22[i][j][k];
        h_d23a[i-n1a][j][k-n3a] = h_d23[i][j][k];
        h_d33a[i-n1a][j][k-n3a] = h_d33[i][j][k];
        s_a[i-n1a][j][k-n3a]    = 0.0f; //zeroing out the new output array
      }
    }
  }

  //chunk 1,0,1
  soscpukernel3d(h_ra, h_d11a, h_d12a, h_d13a, h_d22a, h_d23a, h_d33a, alpha, s_a, n1a, n2a, n3a);

  //from the whole arrays to the 
  //halved arrays
  for(int i=0; i<n1a; i++) {
    for(int j=n2a; j<n2; j++) {
      for(int k=n3a; k<n3; k++) {
        st[i+n1a][j-n2a][k]     = s_a[i][j-n2a][k-n3a]; //copying the results to the total array
        h_ra[i][j-n2a][k-n3a]   = h_r[i][j][k];
        h_d11a[i][j-n2a][k-n3a] = h_d11[i][j][k];
        h_d12a[i][j-n2a][k-n3a] = h_d12[i][j][k];
        h_d13a[i][j-n2a][k-n3a] = h_d13[i][j][k];
        h_d22a[i][j-n2a][k-n3a] = h_d22[i][j][k];
        h_d23a[i][j-n2a][k-n3a] = h_d23[i][j][k];
        h_d33a[i][j-n2a][k-n3a] = h_d33[i][j][k];
        s_a[i][j-n2a][k-n3a]    = 0.0f; //zeroing out the new output array
      }
    }
  }

  //chunk 0,1,1
  soscpukernel3d(h_ra, h_d11a, h_d12a, h_d13a, h_d22a, h_d23a, h_d33a, alpha, s_a, n1a, n2a, n3a);

  //from the whole arrays to the 
  //halved arrays
  for(int i=n1a; i<n1; i++) {
    for(int j=n2a; j<n2; j++) {
      for(int k=n3a; k<n3; k++) {
        st[i-n1a][j][k]             = s_a[i-n1a][j-n2a][k-n3a]; //copying the results to the total array
        h_ra[i-n1a][j-n2a][k-n3a]   = h_r[i][j][k];
        h_d11a[i-n1a][j-n2a][k-n3a] = h_d11[i][j][k];
        h_d12a[i-n1a][j-n2a][k-n3a] = h_d12[i][j][k];
        h_d13a[i-n1a][j-n2a][k-n3a] = h_d13[i][j][k];
        h_d22a[i-n1a][j-n2a][k-n3a] = h_d22[i][j][k];
        h_d23a[i-n1a][j-n2a][k-n3a] = h_d23[i][j][k];
        h_d33a[i-n1a][j-n2a][k-n3a] = h_d33[i][j][k];
        s_a[i-n1a][j-n2a][k-n3a]    = 0.0f; //zeroing out the new output array
      }
    }
  }

  //chunk 1,1,1
  soscpukernel3d(h_ra, h_d11a, h_d12a, h_d13a, h_d22a, h_d23a, h_d33a, alpha, s_a, n1a, n2a, n3a);

  for(int i=n1a; i<n1; i++) {
    for(int j=n2a; j<n2; j++) {
      for(int k=n3a; k<n3; k++) {
        st[i][j][k] = s_a[i-n1a][j-n2a][k-n3a]; //copying the results to the total array
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

  // Freeing chunk 3D arrays
  for(int i=0; i<n1a+2; i++) {
    for(int j=0; j<n2a+2; j++) {
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

