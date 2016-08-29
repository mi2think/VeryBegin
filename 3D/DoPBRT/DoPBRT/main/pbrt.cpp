/********************************************************************
	created:	2016/05/18
	created:	18:5:2016   17:43
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\main\pbrt.cpp
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\main
	file base:	pbrt
	file ext:	cpp
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	main
*********************************************************************/
#include "api.h"
#include "parser.h"

using namespace dopbrt;

int main(int argc, char* argv[])
{
	Options options;
	vector<string> fileNames;
	// process command-line arguments
	// TODO:
	pbrtInit(options);
	// process scene description
	if (fileNames.size() == 0)
	{
		// parse file from standard input0
		ParseFile("-");
	}
	else
	{
		// parse scene from input files
		for (auto& fileName : fileNames)
		{
			if (! ParseFile(fileName)
			{
				LOG_ERROR("Can not open scene file:%s", fileName.c_str());
			}
		}
	}

	pbrtCleanup();

	return 0;
}
