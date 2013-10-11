#include "matAddcpp.h"
#include <iostream>


/**
 * Function that takes two matrices, a and b, and adds them in serial to make c.
 * All three matrices must be the same size.
 */

//typedef vector<float> vfloat1;
//typedef vector< vector<float> > vfloat2;

// Vector Version
void matAddcpp(const vfloat2 &a, const vfloat2 &b, vfloat2 &c) {

  // Obtaining matrix size
  int n1 = a[0].size();
  int n2 = a   .size();

  // Adding elements
  for (int i1=0; i1<n1; ++i1) {
    for (int i2=0; i2<n2; ++i2) {
      c[i1][i2] = a[i1][i2] + b[i1][i2];
    }
  }
}

/*int main()
{

  int n1 = 10;
  int n2 = 11;

  vfloat1 a1(n1,0);
  
  vfloat2 a2(n2,a1);
  vfloat2 b2(a2);
  vfloat2 c2(a2);

  for (int i1=0; i1<n1; ++i1) {
    for (int i2=0; i2<n2; ++i2) {
      a2[i1][i2] = i1;
      b2[i1][i2] = i2;
    }
  }


  matAddcpp(a2,b2,c2);

  for (int i1=0; i1<n1; ++i1) {
    for (int i2=0; i2<n2; ++i2) {
      cout << c2[i1][i2] << " ";
    }
    cout << "  " << c2[i1].size() << endl;
  }

  return 0;
}*/
