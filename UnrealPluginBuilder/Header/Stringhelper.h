#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "BasicTypes.h"

using namespace std;

string ToString(const ifstream& fileStream)
{
	stringstream sstream;
	sstream << fileStream.rdbuf();
	string ret;
	sstream >> ret;
	return ret;
}

void ParseToArray(const string& str, const string& delimiter, vector<string>& outStr)
{
	if (str.length() == 0)
	{
		return;
	}

	if (delimiter.length() == 0)
	{
		outStr.push_back(str);
		return;
	}

	uint32 delimiterLocation = 0;
	uint32 afterLastDelimiter = 0;

	while (delimiterLocation != string::npos && delimiterLocation != str.length() - 1)
	{
		delimiterLocation = str.find(delimiter, afterLastDelimiter);

		if (delimiterLocation != afterLastDelimiter)	// Prevent empty string output
		{
			outStr.push_back(str.substr(afterLastDelimiter, delimiterLocation));
		}

		afterLastDelimiter = delimiterLocation + 1;
	}
}

