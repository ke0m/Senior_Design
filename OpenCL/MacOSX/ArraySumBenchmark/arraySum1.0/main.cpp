#pragma mark -

/*
 * File Name: "FlopsTestGPULoop.cpp"
 * Kernel Code: "flopstest.cl"
 * Author: Joseph Jennings
 * Date: August 17, 2013
 * Description: This code uses the 2D array sum example in order to compare the performance of GPU programming
 * 				and CPU programming. It allows us to see how many FLOPs are required in order to make
 * 				programming the GPU useful.
 */

#include <iostream>
#include <stdlib.h>
#include <OpenCL/OpenCL.h>
#include <sys/time.h>

class GPU{
    
    
    /*
     * These are the declarations of the OpenCL structures are described below:
     * cl_platform-id - Stores the types of platforms installed on the host.
     * cl_device_id - Stores the type of the device (GPU, CPU, etc.)
     * cl_context - Stores the context in which a command queue can be created.
     * cl_command_queue - Stores the command queue which governs how the GPU will
     *                    will execute the kernel.
     * cl_program - Stores the kernel code (which can be comprised of several kernels). Is compiled later its
     * 				functions get packaged into kernels.
     * cl_kernel - The OpenCL data structure that represents kernels.
     */
    
private:
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    
    //A cl_int used to store error flags that are returned if OpenCL function does not execute properly
    cl_int err;
    const char* fileName;
    const char* kernelName;
    
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    
    //The number of work items in each dimension of the data.
    size_t work_units_per_kernel;
    
    //FIXME: Fix the method getProgramSize();
    size_t testSize = 8124;
    
    
    cl_platform_id setPlatform(){
        
        err = clGetPlatformIDs(1, &platform, NULL);
        if (err != CL_SUCCESS){
            std::cout << "Error: Failed to locate the platform." << std::endl;
            exit(1);
        }
        return platform;
    }
    
    
    
    cl_device_id setDevice(){
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Failed to locate the device." << std::endl;
            exit(1);
            
        }
        
        return device;
        
    }
    
    
    cl_context createContext(){
        context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not create a context." << std::endl;
            exit(1);
        }
        
        return context;
    }
    
    
    //TODO: Get this method to function properly. It needs to find the correct size in order to create a big enough buffer.
    size_t findProgramSize(){
        size_t pSize;
        
        if(!program_handle){
            std::cout << "Error: Failed to load Kernel" << std::endl;
            exit(1);
        }
        fseek(program_handle, 0, SEEK_END);
        pSize = ftell(program_handle);
        rewind(program_handle);
        return pSize;
    }
    
    
    char* createProgramBuffer(){
        program_size = findProgramSize();
        program_buffer = (char*)malloc(program_size + 1);
        program_buffer[program_size] = '\0';
        fread(program_buffer, sizeof(char), program_size, program_handle);
        fclose(program_handle);
        
        return program_buffer;
    }

    std::string getProgramInfo(){
        std::string programLog;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        program_log = (char*)malloc(log_size+1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size+1, program_log, NULL);
        
        programLog = program_log;
        
        free(program_log);
        
        return programLog;
    }
    
    //TODO: Once the above method has been modified, change this method so that it works with the changes.
    cl_program buildProgram(){
        program = clCreateProgramWithSource(context, 1, (const char **)&program_buffer, (const size_t *)&program_size, &err);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not create the program" << std::endl;
            exit(1);
        }
        err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not compile the program" << std::endl;
            std::cout << getProgramInfo() << std::endl;
            exit(1);
        }
        
        return program;
        
    }
    
    cl_kernel createKernel(){
        kernel = clCreateKernel(program, "arraysum", &err);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not create the kernel." << std::endl;
            exit(1);
        }
        
        return kernel;
        
    }
    
    cl_command_queue createQueue(){
        queue = clCreateCommandQueue(context, device, 0, &err);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not create the queue." << std::endl;
            exit(1);
        }
        
        return queue;
    }
    



public:
    GPU(const char* kernelFileName, const char* nameOfKernel){
        
       
        err = 0;
        fileName = kernelFileName;
        kernelName = nameOfKernel;
        // TODO: Get this working the private methods written above.
        //program_handle = fopen("arraysum.cl", "r");
        
        platform = setPlatform();
        
        device = setDevice();
        
        context = createContext();
        
        program_buffer = createProgramBuffer();
        
        program = buildProgram();
        
        kernel = createKernel();
    
        queue = createQueue();
        
    }
    
    
    cl_platform_id getPlatform(){
        
        return platform;
    }
    
    cl_device_id getDevice(){
        
        return device;
    }
    
    cl_context getContext(){
        
        return context;
    }
    
    cl_command_queue getQueue(){
        
        return queue;
    }
    
    cl_kernel getKernel(){
        
        return kernel;
    }
    
    cl_program  getProgram(){
        
        return program;
    }

    cl_int getErr(){
        
        return err;
    }

};



void arraySumGPU(int n, cl_int err, cl_kernel kernel, cl_mem d_xx, cl_mem d_yy, cl_mem d_zz, cl_command_queue queue, float **h_xx, float **h_yy, float **h_zz, size_t work_units_per_kernel)
{
    
    //Start While Loop
    
 
    
    for(int i = 0; i<n; i++)
    {
        //Writing the data from the host to the device
        err = clEnqueueWriteBuffer(queue, d_xx, CL_TRUE, 0, sizeof(float)*n, h_xx[i], 0, NULL, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not write to buffer d_xx" << std::endl;
            exit(1);
        }
        
        err = clEnqueueWriteBuffer(queue, d_yy, CL_TRUE, 0, sizeof(float)*n, h_yy[i], 0, NULL, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not write to buffer d_yy" << std::endl;
            exit(1);
        }
        
        //Setting the Kernel Arguments
        err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_xx);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not set kernel argument h_xx." << std::endl;
            exit(1);
        }
        
        err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_yy);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not set kernel argument h_yy." << std::endl;
            exit(1);
        }
        
        err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_zz);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not set kernel argument h_zz." << std::endl;
        }
        
        work_units_per_kernel = n;
        
        //Executing the Kernel
        err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &work_units_per_kernel, NULL, 0, NULL, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not execute kernel." << std::endl;
            exit(1);
        }
        
        //Reading the Data from the Kernel
        err = clEnqueueReadBuffer(queue, d_zz, CL_TRUE, 0, n*(sizeof(float)), h_zz[i], 0, NULL, NULL);
        if(err != CL_SUCCESS){
            std::cout << "Error: Could not read data from kernel." << std::endl;
            exit(1);
        }
        
    }
    
    // End While loop
    
}

void packArray(int n1, int n2, float ** array2d, float * array1d){
    
    int k = 0;
    array1d = new float[n1*n2];
   // array1d = (float*)(malloc(sizeof(float)*n1*n2));
    
    for (int i = 0; i < n1; i++){
        for (int j = 0; j < n2; j++){
            
            array1d[k] = array2d[i][j];
            k++;
        }
        
    }
    
    for (int i=0; i<(n1*n2); i++)
        std::cout << array1d[i] << std::endl;
    
}



void unpackArray(int n1, int n2, float* array1d, float** array2d){

    int k = 0;
    
    array2d = (float**)malloc(sizeof(float*)*n1);
    
    for (int i=0; i < n1; i++)
        array2d[i] = (float*)malloc(sizeof(float)*n2);
    
    for (int i = 0; );
    
    
}



int main(int argc, const char * argv[])
{
    //TODO: Constructor does not currently work with arguments. Fix so that it does.
    GPU test("flopstestloop.cl", "arraysum");
    
    //The number of work items in each dimension of the data.
    size_t work_units_per_kernel = 0;
    
	//This value determines the size of the nxn (square) array.
    int n = 2;
    
	//Allocating the memory for the nxn arrays of floats.
    float **h_xx = (float**)malloc(sizeof(float*)*n);
    float **h_yy = (float**)malloc(sizeof(float*)*n);
    float **h_zz = (float**)malloc(sizeof(float*)*n);
    
    for(int i = 0; i<n; i++){
        h_xx[i] = (float*)malloc(sizeof(float)*n);
        h_yy[i] = (float*)malloc(sizeof(float)*n);
        h_zz[i] = (float*)malloc(sizeof(float)*n);
        
		//Initializing the arrays.
        for(int j = 0; j<n; j++){
            
            h_xx[i][j] = i+j;
            h_yy[i][j] = i+j;
            
        }
        
    }
    
    
    cl_mem d_xx;
    cl_mem d_yy;
    cl_mem d_zz;
    
    cl_int err = test.getErr();
    cl_context context = test.getContext();
    cl_kernel kernel = test.getKernel();
    cl_program program = test.getProgram();
    cl_command_queue queue = test.getQueue();
    
    //Creating the Device memory buffers. These will be used to transfer data from the host to the device and vice versa.
    d_xx = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*n, NULL, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the buffer d_xx" << std::endl;
        exit(1);
    }
    
    d_yy = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*n, NULL, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the buffer d_yy" << std::endl;
        exit(1);
    }
    
    d_zz = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*n, NULL, &err);
    if(err != CL_SUCCESS){
        std::cout << "Error: Could not create the buffer d_zz" << std::endl;
        exit(1);
    }

    
    
    timeval tim1, tim2;
    gettimeofday(&tim1, NULL);
    
    u_int64_t initialTime = tim1.tv_sec;
    
    u_int64_t maxTime = 3000*1000000.00;
    
    u_int64_t elapsedTime = 0;
    
    double counter = 0;
    

    arraySumGPU(n, err, kernel,d_xx, d_yy, d_zz, queue, h_xx, h_yy, h_zz, work_units_per_kernel);
    
    
    while (elapsedTime < maxTime)
    {
    
        arraySumGPU(n, err, kernel,d_xx, d_yy, d_zz, queue, h_xx, h_yy, h_zz, work_units_per_kernel);
        gettimeofday(&tim2, NULL);
        elapsedTime = tim2.tv_sec - initialTime;
        counter++;
   }
    
    //Testing the function packArray
    int n1 = 2;
    int n2 = 3;
    
    float **array2d = (float**)malloc(sizeof(float*)*n1);
    
    
    for(int i = 0; i<n1; i++){
        array2d[i] = (float*)malloc(sizeof(float)*n2);

        for(int j = 0; j<n1; j++){
            
            array2d[i][j] = i+j;

        }
        
    }
    
    float* array1d;
    
    packArray(n1, n2, array2d, array1d);
        
    //Testing the function unpackArray
    
    
    return 0;

}







