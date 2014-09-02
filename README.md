## Structure-Oriented Smoothing of 2D Images on the GPU

This repository contains the code for Joseph Jennings' Senior Design Project (GPGN 438) 
under the guidance of Professor [Dave Hale](http://inside.mines.edu/~dhale) 
at the [Colorado School of Mines](http://mines.edu).

This software builds upon the  [Mines Java Toolkit (JTK)](https://github.com/dhale/jtk/) which 
will first need to be downloaded and built in order to run the actual Structure-Oriented Smoothing software 
contained within this repository. Specifically, the final product of this software
is a small extension within the LocalSmoothingFilter class of the Mines JTK.
It utilizes the [JOCL](http://www.jocl.org/) libraries (Java bindings for OpenCL) to multi-thread the 
LocalSmoothingFilter kernel on a GPU (Graphics Processing Unit). 

The remaining software consists mostly of toy JOCL and OpenCL programs to I used to familiarize 
myself with the OpenCL library.


### Personal Summary

This summary is an attempt for me to organize the code that I finished writing in May 2014.

