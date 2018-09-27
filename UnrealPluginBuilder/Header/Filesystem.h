#pragma once

#include <string>
#include <sys/stat.h>

using namespace std;

bool FileExists(const string& file)
{
	struct stat buffer;
	return (stat(file.c_str(), &buffer) == 0);
}