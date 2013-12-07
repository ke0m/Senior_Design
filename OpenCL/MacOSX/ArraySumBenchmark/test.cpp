
#include <iostream>

#include <vector>

#include <stdlib.h>

#include "ArraySumUtil.h"


int main(){

	ArraySumUtil junk;
	int n1 = 3;
	int n2 = 2;
	std::vector<float> vec1d(n1*n2);
	std::vector<std::vector<float> > vec2d(n1, std::vector<float>(n2));


	for (int i = 0; i < n1; ++i)
		for (int j = 0; j < n2; ++j)
			vec2d[i][j] = i + j;

	junk.printVec2D(vec2d);
	std::cout << std::endl;
	junk.packVector(vec2d, vec1d);

	for (int i = 0; i < n1*n2; ++i)
		std::cout << vec1d[i] << std::endl;

	junk.unPackVector(vec1d, vec2d);

	junk.printVec2D(vec2d);

	return 0;

}




