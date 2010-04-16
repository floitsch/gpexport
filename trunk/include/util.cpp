#include "util.h"
#include <maya/MFnDependencyNode.h>
#include <maya/MObjectArray.h>



void printMObjectArray(const std::string& tips, const MObjectArray& array)
{
	std::cout << tips ;
	for(int i = 0; i < array.length(); ++i)
	{
		std::cout <<  MFnDependencyNode(array[i]).name().asChar() << ", ";
	}std::cout << std::endl;
}