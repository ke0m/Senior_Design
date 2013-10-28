//#include "test.h"
#include <iostream>
#include <vector>
#include "Stopwatch.h"

using namespace std;

int main()
{
  Stopwatch sw;
  sw.restart();
  int maxtime = 2;
  while(sw.getTime()<maxtime) {
    printf("%f \n",sw.getTime());
  }
  sw.stop();
  printf("end time = %f \n",sw.getTime());
} 
