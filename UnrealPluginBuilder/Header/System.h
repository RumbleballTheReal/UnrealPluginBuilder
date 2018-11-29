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
		if (command.find("\\") != string::npos || command.find("/") != string::npos)
		{
			string wrapped;
			wrapped.append("\"\"").append(command).append("\" ").append(arguments).append("\"");
			return system(wrapped.c_str());
		}
		else
		{
			string concat;
			concat.append(command).append(" ").append(arguments);
			return system(concat.c_str());
		}

	}

	

}