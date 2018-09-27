#pragma once

#include <cstdlib>
#include <string>

using namespace std;

namespace System
{
	void Pause()
	{
		system("pause");
	}

	int Exec(const string& command, const string& arguments)
	{
		string wrapped;
		wrapped.append("\"\"").append(command).append("\" ").append(arguments).append("\"");
		return system(wrapped.c_str());
	}

}