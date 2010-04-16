#ifndef _UTIL_
#define _UTIL_

#include <string>
#include <iostream>

class MObjectArray;


template< typename MArrayT >
void printArray(const std::string& tips, const MArrayT& array)
{
	std::cout << tips ;
	for(int i = 0; i < array.length(); ++i)
	{
		std::cout << array[i] << ", ";
	}std::cout << std::endl;
}

void printMObjectArray(const std::string& tips, const MObjectArray& array);

#endif