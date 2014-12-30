#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <CL/opencl.h>
#include <string.h>
#include <math.h>
#include "Stopwatch.h"

int convertToString(const char *filename, std::string& s)
{
  size_t size;
  char*  str; 
  std::fstream f(filename, (std::fstream::in | std::fstream::binary));

  if(f.is_open())
  {
    size_t fileSize;
    f.seekg(0, std::fstream::end);
    size = fileSize = (size_t)f.tellg();
    f.seekg(0, std::fstream::beg);
    str = new char[size+1];
    if(!str)
    {    
      f.close();
      return 0;
    }    

    f.read(str, fileSize);
    f.close();
    str[size] = '\0';
    s = str; 
    delete[] str; 
    return 0;
  }
  std::cout<<"Error: failed to open file\n:"<<filename<<std::endl;
  return 1;
}

int main(int argc, const char * argv[]) {

  const int n1   = 64;
  const int n2   = n1+2;
  const int n3   = n2+2;
  const int dims = n1*n2*n3;
  float value    = 0.0f;

  const char* fileName;

  char *program_log;
  size_t log_size;
  std::string programLog;

  //OpenCL declaration and initialization
  cl_platform_id platform;
  cl_device_id device;
  cl_context context;
  cl_command_queue queue;
  cl_program program;
  cl_kernel kernel0, kernel1;
  cl_uint num_platforms;
  cl_int err;

  err = clGetPlatformIDs(1, &platform, &num_platforms);
  if (err != CL_SUCCESS){
    std::cout << "Error: Failed to locate the platform." << std::endl;
    system("pause");
    exit(1);
  }

  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
  if(err != CL_SUCCESS){
    std::cout << "Error: Failed to locate the device." << std::endl;
    std::cout << "OpenCL Error code: " << err << std::endl;
    system("pause");
    exit(1);

  }
  context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create a context." << std::endl;
    system("pause");
    exit(1);
  }

  fileName = "ref_sosmoothing3d.cl";
  std::string sourceStr;
  int status = convertToString(fileName, sourceStr);
  if(status == 1) {
    system("pause");
    exit(1);
  }
  const char *source = sourceStr.c_str();
  size_t sourceSize[] = {strlen(source)};
  program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the program" << std::endl;
    system("pause");
    exit(1);
  }

  err = clBuildProgram(program, 1, &device, "-cl-strict-aliasing -cl-unsafe-math-optimizations -cl-finite-math-only -cl-fast-relaxed-math -DMAC", NULL, NULL); //here I use the cl-opt-disable flag to disable the FMA rounding. This was the only way I could get both CPU and GPU output to match
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not compile the program" << std::endl;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    program_log = (char*)malloc(log_size+1);
    program_log[log_size] = '\0';
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size+1, program_log, NULL);
    programLog = program_log;
    std::cout << programLog << std::endl;
    free(program_log);
    system("pause");
    exit(1);
  }

  kernel0 = clCreateKernel(program, "setMem", &err);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the kernel." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    system("pause");
    exit(1);
  }

  kernel1 = clCreateKernel(program, "soSmoothingNew3d", &err); 
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the kernel." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    system("pause");
    exit(1);
  }

  queue = clCreateCommandQueue(context, device, 0, &err);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the queue." << std::endl;
    system("pause");
    exit(1);
  }

  std::ofstream inputData;
  std::ofstream cpuOutput;
  std::ofstream gpuOutput;
  cpuOutput.open("cpuoutput.txt");
  gpuOutput.open("gpuoutput.txt");
  inputData.open("inputData.txt");

  //Memory allocation and initliaziation
  float ***h_r   =  new float**[n1];
  float ***h_s   =  new float**[n1];
  float ***s     =  new float**[n1];
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
      }
    }
  }

  //Packing the arrays
  float *h_r1    = new float[dims];
  float *h_s1    = new float[(n1+1)*(n2+1)*(n3+1)];
  float *h_d11_1 = new float[dims];
  float *h_d12_1 = new float[dims];
  float *h_d13_1 = new float[dims];
  float *h_d22_1 = new float[dims];
  float *h_d23_1 = new float[dims];
  float *h_d33_1 = new float[dims];
  float *test    = new float[dims];

  int idx = 0;
  for(int i=0; i<n1; i++) {
    for(int j=0; j<n2; j++) {
      for(int k=0; k<n3; k++) {
        idx = i+j*n1+k*n1*n2;
        h_r1[idx]    = h_r[i][j][k];
        h_s1[idx]    = s[i][j][k];
        h_d11_1[idx] = h_d11[i][j][k];
        h_d12_1[idx] = h_d12[i][j][k];
        h_d13_1[idx] = h_d13[i][j][k];
        h_d22_1[idx] = h_d22[i][j][k];
        h_d23_1[idx] = h_d23[i][j][k];
        h_d33_1[idx] = h_d33[i][j][k];
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

  cl_mem d_s, d_r, d_d11, d_d12, d_d13, d_d22, d_d23, d_d33;

  d_r = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the buffer." << std::endl;
    std::cout << "OpenCL Error code: " << err << std::endl;
    system("pause");
    exit(1);
  }    

  d_d11 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the buffer." << std::endl;
    std::cout << "OpenCL Error code: " << err << std::endl;
    system("pause");
    exit(1);
  }    

  d_d12 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the buffer." << std::endl;
    std::cout << "OpenCL Error code: " << err << std::endl;
    system("pause");
    exit(1);
  }    

  d_d13 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the buffer." << std::endl;
    std::cout << "OpenCL Error code: " << err << std::endl;
    system("pause");
    exit(1);
  }

  d_d22 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the buffer." << std::endl;
    std::cout << "OpenCL Error code: " << err << std::endl;
    system("pause");
    exit(1);
  }    

  d_d23 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the buffer." << std::endl;
    std::cout << "OpenCL Error code: " << err << std::endl;
    system("pause");
    exit(1);
  }    

  d_d33 = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*dims, NULL, &err);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the buffer." << std::endl;
    std::cout << "OpenCL Error code: " << err << std::endl;
    system("pause");
    exit(1);
  }   

  d_s = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*(n1+1)*(n2+1)*(n3+1), NULL, &err);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not create the buffer." << std::endl;
    std::cout << "OpenCL Error code: " << err << std::endl;
    system("pause");
    exit(1);
  }

  err = clEnqueueWriteBuffer(queue, d_r, CL_FALSE, 0, sizeof(float)*dims, h_r1, 0, NULL, NULL);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not write to buffer." << std::endl;
    std::cout << "OpenCL Error Code: " << std::endl;
    system("pause");
    exit(1);
  }  

  err = clEnqueueWriteBuffer(queue, d_d11, CL_FALSE, 0, sizeof(float)*dims, h_d11_1, 0, NULL, NULL);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not write to buffer." << std::endl;
    std::cout << "OpenCL Error Code: " << std::endl;
    system("pause");
    exit(1);
  }

  err = clEnqueueWriteBuffer(queue, d_d12, CL_FALSE, 0, sizeof(float)*dims, h_d12_1, 0, NULL, NULL);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not write to buffer." << std::endl;
    std::cout << "OpenCL Error Code: " << std::endl;
    system("pause");
    exit(1);
  }  

  err = clEnqueueWriteBuffer(queue, d_d13, CL_FALSE, 0, sizeof(float)*dims, h_d13_1, 0, NULL, NULL);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not write to buffer." << std::endl;
    std::cout << "OpenCL Error Code: " << std::endl;
    system("pause");
    exit(1);
  }

  err = clEnqueueWriteBuffer(queue, d_d22, CL_FALSE, 0, sizeof(float)*dims, h_d22_1, 0, NULL, NULL);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not write to buffer." << std::endl;
    std::cout << "OpenCL Error Code: " << std::endl;
    system("pause");
    exit(1);
  }  

  err = clEnqueueWriteBuffer(queue, d_d23, CL_FALSE, 0, sizeof(float)*dims, h_d23_1, 0, NULL, NULL);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not write to buffer." << std::endl;
    std::cout << "OpenCL Error Code: " << std::endl;
    system("pause");
    exit(1);
  }  

  err = clEnqueueWriteBuffer(queue, d_d33, CL_FALSE, 0, sizeof(float)*dims, h_d33_1, 0, NULL, NULL);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not write to buffer." << std::endl;
    std::cout << "OpenCL Error Code: " << std::endl;
    system("pause");
    exit(1);
  } 


  err = clSetKernelArg(kernel0, 0, sizeof(cl_mem), &d_s);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }    

  err = clSetKernelArg(kernel0, 1, sizeof(float), &value);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }    


  err = clSetKernelArg(kernel0, 2, sizeof(int), &dims);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }    

  //First kernel in the program: used in order to zero out the input s array
  size_t oned_global_work_size = dims;
  err = clEnqueueNDRangeKernel(queue, kernel0, 1, NULL, &oned_global_work_size, NULL, 0, NULL, NULL);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not execute the kernel." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    system("pause");
    exit(1);
  }

  size_t local_group_size[3] = {8,8,8};
  size_t mapped_n1 = n2/2;
  size_t mapped_n2 = n1/2;
  size_t mapped_n3 = n3/2;
  size_t global_group_size_block[3] = {ceil((mapped_n1/local_group_size[0]) + 1) * local_group_size[0], ceil((mapped_n2/local_group_size[1]) + 1) * local_group_size[1], ceil((mapped_n3/local_group_size[2] + 1) *local_group_size[2])}; 


  err = clSetKernelArg(kernel1, 0, sizeof(cl_mem), &d_r);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }

  err = clSetKernelArg(kernel1, 1, sizeof(cl_mem), &d_d11);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }

  err = clSetKernelArg(kernel1, 2, sizeof(cl_mem), &d_d12);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }

  err = clSetKernelArg(kernel1, 3, sizeof(cl_mem), &d_d13);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }    

  err = clSetKernelArg(kernel1, 4, sizeof(cl_mem), &d_d22);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }

  err = clSetKernelArg(kernel1, 5, sizeof(cl_mem), &d_d23);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }    

  err = clSetKernelArg(kernel1, 6, sizeof(cl_mem), &d_d33);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }    

  err = clSetKernelArg(kernel1, 7, sizeof(cl_mem), &d_s);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }

  err = clSetKernelArg(kernel1, 8, sizeof(float), &alpha);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }

  err = clSetKernelArg(kernel1, 9, sizeof(int), &n1);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }

  err = clSetKernelArg(kernel1, 10, sizeof(int), &n2);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }

  err = clSetKernelArg(kernel1, 11, sizeof(int), &n3);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not set the kernel argument." << std::endl;
    std::cout << "OpenCL error code: " << err << std::endl;
    exit(1);
  }

  for (int offsetx = 0; offsetx < 2; ++offsetx) {
    for (int offsety = 0; offsety < 2; ++offsety) {
      for(int offsetz = 0; offsetz < 2; ++offsetz) {
        err = clSetKernelArg(kernel1, 12, sizeof(int), &offsetx);
        if(err != CL_SUCCESS){
          std::cout << "Error: Could not set the kernel argument." << std::endl;
          std::cout << "OpenCL error code: " << err << std::endl;
          exit(1);
        }
        err = clSetKernelArg(kernel1, 13, sizeof(int), &offsety);
        if(err != CL_SUCCESS){
          std::cout << "Error: Could not set the kernel argument." << std::endl;
          std::cout << "OpenCL error code: " << err << std::endl;
          exit(1);
        }
        err = clSetKernelArg(kernel1, 14, sizeof(int), &offsetz);
        if(err != CL_SUCCESS){
          std::cout << "Error: Could not set the kernel argument." << std::endl;
          std::cout << "OpenCL error code: " << err << std::endl;
          exit(1);
        }

        err = clEnqueueNDRangeKernel(queue, kernel1, 3, NULL, global_group_size_block, local_group_size, 0, NULL, NULL);
        if(err != CL_SUCCESS){
          std::cout << "Error: Could not execute the kernel." << std::endl;
          std::cout << "OpenCL error code: " << err << std::endl;
          system("pause");
          exit(1);
        }
      }
    }    
  }    

  err = clEnqueueReadBuffer(queue, d_s, CL_TRUE, 0, sizeof(float)*(n1+1)*(n2+1)*(n3+1), h_s1, 0, NULL, NULL);
  if(err != CL_SUCCESS){
    std::cout << "Error: Could not read data from the device." << std::endl;
    std::cout << "OpenCL error code: " << std::endl;
    exit(1);
  }
  
  //Unpacking the array
  for(int i=0; i<n1; i++) {
    for(int j=0; j<n2; j++) {
      for(int k=0; k<n3; k++) {
        h_s[i][j][k] = h_s1[i+j*n1+k*n1*n2];
      }
    }
  }
  
  //Verify the results
  bool check = true;
  int numfails = 0;
  for(int i=0; i<n1; i++) {
    for(int j=0; j<n2; j++) {
      for(int k=0; k<n3; k++) {
        if(fabs(s[i][j][k] - h_s[i][j][k]) < 0.001f) {
          continue;
        }
        else {
          if(numfails < 100) {
            printf("Check failed at array element: (%d, %d, %d)\n", i, j, k);
            printf("CPU Value: %f\n", s[i][j][k]);
            printf("GPU Value: %f\n", h_s[i][j][k]);
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

  //Releasing allocated memory on the GPU
  clReleaseMemObject(d_r);
  clReleaseMemObject(d_s);
  clReleaseMemObject(d_d11);
  clReleaseMemObject(d_d12);
  clReleaseMemObject(d_d22);
  clReleaseKernel(kernel0);
  clReleaseKernel(kernel1);
  clReleaseCommandQueue(queue);
  clReleaseProgram(program);
  clReleaseContext(context);


  //Writing out the computed values for plotting
  for(int i=0; i<n1; i++) {
    for(int j=0; j<n2; j++) {
      for(int k=0; k<n3; k++) {
        inputData << h_r[i][j][k] << " ";
        cpuOutput << s[i][j][k]   << " ";
        gpuOutput << h_s[i][j][k] << " ";
      }
      inputData << std::endl;
      cpuOutput << std::endl;
      gpuOutput << std::endl;
    }
    inputData << std::endl << std::endl;
    cpuOutput << std::endl << std::endl;
    gpuOutput << std::endl << std::endl;
  }

  gpuOutput.close();
  cpuOutput.close();
  inputData.close();

  // Freeing 3D arrays
  for(int i=0; i<n1; i++) {
    for(int j=0; j<n2; j++) {
      delete [] h_r[i][j]; 
      delete [] h_s[i][j]; 
      delete [] s[i][j]; 
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
  delete [] h_d11; 
  delete [] h_d12; 
  delete [] h_d13; 
  delete [] h_d22; 
  delete [] h_d23; 
  delete [] h_d33; 

  //Freeing 1D arrays
  delete [] h_r1;
  delete [] h_s1;
  delete [] h_d11_1;
  delete [] h_d12_1;
  delete [] h_d13_1;
  delete [] h_d22_1;
  delete [] h_d23_1;
  delete [] h_d33_1;
  delete [] test;

  //Plotting the results with python mayavi
  system("python volume_slicer.py &");
  system("python volume_slicer_cpu.py &");
  system("python volume_slicer_gpu.py &");
  system("python volume_slicer_diff.py &");

  return 0;

}
