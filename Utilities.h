#pragma once
#include <string>
#include <vector>

::std::vector< ::std::string> ReadLinesFromFile(const ::std::string& pathToFile);

::std::vector< double> DoubleVectorFromString(const ::std::string& inputString);

::std::vector<double> randVector(int N);	// generate N ramdom values uniformly between 0 and 1

void tic();		// start time
double toc();	// get time from tic() in secs.