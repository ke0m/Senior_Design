## Structure-Oriented Smoothing of 2D Images on the GPU

This repository contains the code for Joseph Jennings' Senior Design Project (GPGN 438) 
under the guidance of Professor [Dave Hale](http://inside.mines.edu/~dhale) 
at the [Colorado School of Mines](http://mines.edu).

This software builds upon the  [Mines Java Toolkit (JTK)](https://github.com/dhale/jtk/) which 
will first need to be downloaded and built in order to run the actual Structure-Oriented Smoothing software 
contained within this repository. Specifically, the final product of this software
is a small extension within the `LocalSmoothingFilter.java` class of the Mines JTK that was first published in Professor 
Hale's paper [Structure-oriented smoothing and semblance](http://inside.mines.edu/~dhale/papers/Hale09StructureOrientedSmoothingAndSemblance.pdf).
My extension utilizes the [JOCL](http://www.jocl.org/) libraries (Java bindings for OpenCL) to multi-thread the 
LocalSmoothingFilter kernel on a GPU (Graphics Processing Unit). 

The remaining software consists mostly of toy JOCL and OpenCL programs I used to familiarize 
myself with the OpenCL library and image processing.

### Personal Summary (a log for myself)

This summary is an attempt for me to organize the code that I finished writing in May 2014.

####OpenCL

In each of the programs written in this directory, C++ was the language used to call the OpenCL 
libraries. The software is provided for Linux, OSX, and Microsoft Windows operating systems.

##### MacOSX

The first programs that I wrote in OpenCL I did on a MacBook Pro running OSX Lion with an
[NVIDIA GeForce 9400M 256 MB](http://www.geforce.com/hardware/notebook-gpus/geforce-9400mg)
graphics card. These programs are 'toy' programs
that I wrote to teach myself the basics of parallel computing for multi-core CPUs and GPUs. 
An example of a toy program in this directory is the `ArraySum.cpp` program that adds two
2D arrays in parallel on the GPU.

##### Linux

After getting frustrated with the graphics card on my MacBook Pro (limited amount of memory, etc.) 
I moved to a Linux machine on which I could install my own graphics card. 
First, I began testing on an [NVIDIA GT 640](http://www.geforce.com/hardware/desktop-gpus/geforce-gt640) 
by porting the programs I had written in C++ and OpenCL from MacOSX to Ubuntu Linux. Later, I upgraded to an
[AMD Radeon HD 7870](http://www.amd.com/en-gb/products/graphics/desktop/7000/7800#) and continued to test
my toy programs on this graphics card with much more memory and also GPU cores.

##### Windows

After getting some functional toy programs, I also ported them to Windows and started to 'tweak' them for really
high performance. Now testing on an [Asus ROG laptop](http://rog.asus.com/tag/gtx-765m/) 
with an [NVIDIA GeForce GTX 765M](http://www.geforce.com/hardware/notebook-gpus/geforce-gtx-765m), 
I was able to achieve a performance of approximately 1.0 TFLOPS on this card. The program that I wrote
that accomplishes this is found in `OpenCL/Windows/Senior_Design/ArraySumTotal.cpp`. It uses the 
`winflopf16memfma.cl` utilizing a local group size of 1024 as well as the OpenCL vector data type - 
`float 16`.

Another sub-directory that is within this windows directory is the `flops_results_files`. This directory contains the python
script `perfplot.py` written by [Spencer Haich](https://github.com/spa1ch) that plots the performance of array sum program
versus array size and number of iterations completed on the GPU.

After having familiarized myself with the toy programs, I moved onto the smoothing kernel for 2D images found in Professor
Hale's paper. I tested this first on random numbers. The main working program that accomplishes this is the 
`Refined_SOS_Random_Numbers/Refined_SOS_Rand_Nums.cpp`. This program takes a random image and applies the smoothing kernel to each sample
within the image in parallel on the GPU. I also have written a version of this code that works serially on the CPU in C++.

Additionally, I have written several python scripts that utilize the python plotting library known
as Matplotlib to plot the results for comparison between CPU and GPU output as well as the performance of the 
smoothing kernel in both C++ and OpenCL for a range of array sizes.

####JOCL

The JOCL directory also contains the same toy programs as well as the image processing of random numbers that were implemented using
C++ to call OpenCL. These include:

1. `SOS_Random_Nums`
2. `ArraySum`
3. `FlopsTestGPULoop`

These `OpenCL/Windows` and `JOCL` directories differ in that the `JOCL` directory uses the Mines JTK in order to actually smooth along
computed structure tensors. In order to do this, Java code that calls OpenCL kernels must be included within the `LocalSmoothingFilter.java`
and `LocalDiffusionKernel.java` classes. First, a OpenCL utility class was written in order to make and seamless calls to OpenCL functions.
This class is names `CLUtil.java` and is found within the util directory of the Mines JTK. The functions within `CLUtil.java` are then used
to use JOCL and parallelize the execution of the smoothing kernel on the GPU. Smoothing of random images is done within the 
`LocalSmoothingFilter.java` and `LocalDiffusionKernel.java` programs themselves (inside a test main method).

Lastly, the sub-directory `Real_Image_Smoothing` uses the Jython language to call these extensions made within `LocalSmoothingFilter.java`
and `LocalDiffusionKernel.java` in order to smooth an 
[actual seismic image](https://github.com/ke0m/Senior_Design/blob/master/JOCL/Real_Image_Smoothing/png/input.png)
that was provided to me by Professor Hale.


#### All code: May 5, 2014

This directory was a desperate attempt to save all of the code just in case I lost it. Really, it has everything that I have described before, toy programs, basic image processing, the smoothing kernel of random numbers and the smoothing of random images. In addition, after
I had programmed the smoothing kernel in OpenCL, I programmed the outer conjugate gradient loop in OpenCL as well in order to reduce the 
amount of I/O in my code over the PCI bus (one of the biggest bottlenecks currently in GPU programming). Therefore, I have multiple
versions of the `Real_Image_Smoothing` that differ in the use of object oriented programming as well as this conjugate gradient method
written in OpenCL. Lastly, it has a demo.py (Jython) script that I used to compare the differences in performance of using 
JOCL vs Java in my final presentation to the faculty at CSM for this project.


### Plan going forward

Now that I have a better knowledge of what goes where, I think I will create a new repository  called `soscl` which will be a clean version
of my old `Senior_Design` repository. Here I will create the layout for the repository and I will start to make it on my Asus machine
(where all the code is currently). I will then run each program and clean them up so they have a sufficient amount of comments and are 
formatted nicely. With each program, I will put it into the new repository and make commits everyday.

#### Directory Structure

##### soscl




