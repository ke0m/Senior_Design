//

//  BenchmarkCPU.cpp

//  ArraySum1.1

//

//  Created by Joseph Jennings on 10/2/13.

//  Copyright (c) 2013 Joseph Jennings. All rights reserved.

//

 


#include <iostream>

#include <istream>

#include "ArraySumUtil.h"

#include "Stopwatch.h"

#include <stdlib.h>
 
int main(int argc, const char * argv[])
{

    //ArraySumUtil hope;

    Stopwatch sw;

   // int n1=12000;

    //int n2=20000;

 
       int n1;

    int n2;

  // std::cout << "Please enter array1 size and arrays siz ";


   std::cout << "argv[2]" << argv[2] << std::endl;

 

   

  n1 = atoi(argv[1]);

  n2 = atoi(argv[2]);

 

  // std::cin >> n2 ;

 
   //std::cout << "Please enter array2 size: ";

   //std::cin >> n2;

 

   std::cout << "array1 size is " << n1 << std::endl;

   std::cout << "array2 size is " << n2 << std::endl;


    float **h_xx = (float**)malloc(sizeof(float*)*n1);

    float **h_yy = (float**)malloc(sizeof(float*)*n1);


    std::cout << "Step 1 - Initializing arrays " <<  std::endl;

    for(int i = 0; i<n1; i++){

        h_xx[i] = (float*)malloc(sizeof(float)*n2);

        h_yy[i] = (float*)malloc(sizeof(float)*n2);

        //Initializing the arrays.

        for(int j = 0; j<n2; j++){

            h_xx[i][j] = i+j;

            h_yy[i][j] = i+j;

        }
       
    }


    int maxTime = 10;

    int count = 0;

    std::cout << "Step 2 - Starting stopwatch " <<  std::endl;
 
    sw.start();
    std::cout << "Step 3 -  Stopwatch started" <<  std::endl;
    while (sw.getTime() < maxTime){

        //hope.arraySumCPU(h_xx, h_yy, n1, n2);

              std::cout << "Step 3a -  Inside while loop" <<  std::endl;

              std::cout << "Time before for loop:" << sw.getTime()   <<  std::endl;

    for(int i = 0; i<n1; i++){

        for(int j = 0; j<n2; j++){

             h_xx[i][j] =

                      h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                       + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                     + h_xx[i][j] + h_yy[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j] + h_xx[i][j]

                    

                      

                      ;

              // std::cout << "j:" << j << std::endl;

        }

 

              //std::cout << "i:" << i << std::endl;

       }

 

        count++;

        std::cout << "Time after for loop: " << sw.getTime() << std::endl;

 

              std::cout << "Count" << count << std::endl;

 

 

       

    }

    sw.reset();

   

       std::cout << "Step 4 - Stoping stopwatch " <<  std::endl;

       std::cout << (maxTime) << " MaxTime" << std::endl;

 

 

    float n1f = (float) n1;

    float n2f = (float) n2;

    float countf = (float) count;

   

    

    std::cout << (n1f*n2f*1280*countf*1e-06/maxTime) << " MegaFLOPS" << std::endl;

   

 

   

}
