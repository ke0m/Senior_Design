#include <iostream>
#include <stdlib.h>
#include <vector>
#include <CL\cl.h>



int main(void)
{

	cl_uint platformIdCount = 0;
	clGetPlatformIDs(0, NULL, &platformIdCount);

	char pformvendor[40];

	std::vector<cl_platform_id> platformIds (platformIdCount);
	clGetPlatformIDs(platformIdCount, platformIds.data (), NULL);
	
	for(int i = 0; i < platformIdCount; i++)
	{
		clGetPlatformInfo(platformIds[i], CL_PLATFORM_VENDOR, sizeof(pformvendor), &pformvendor, NULL);
		printf("%s\n",pformvendor);
	}

	system("pause");
	return 0;

}