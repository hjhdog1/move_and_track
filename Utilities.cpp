#include "Utilities.h"

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

clock_t start = ::std::clock();

::std::vector<::std::string> ReadLinesFromFile(const ::std::string& pathToFile)
{
	::std::vector< ::std::string> linesVector;

	::std::ifstream inputFile(pathToFile.c_str());
	
	::std::string tempLine;
	while(::std::getline(inputFile, tempLine))
		linesVector.push_back(tempLine);

	return linesVector;
}


::std::vector< double> DoubleVectorFromString(const ::std::string& inputString)
{
	::std::istringstream ss(inputString);

	::std::vector<double> result;
	while(!ss.eof())
	{
		double tmp;
		ss >> tmp;
		result.push_back(tmp);
	}

	return result;
}

::std::vector<double> randVector(int N)
{
	::std::vector<double> randValues(N);
	srand (time(NULL));
	for(int i = 0 ; i < N ; i++)
		randValues[i] = (double)rand() / RAND_MAX;

	return randValues;
}

void tic()
{
	start = ::std::clock();
}

double toc()
{
	clock_t end = ::std::clock();
	double t = (end - start)/(double)CLOCKS_PER_SEC;
	return t;
}