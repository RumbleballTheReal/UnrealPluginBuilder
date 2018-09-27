#pragma once

#include <fstream>
#include <string>
#include <sstream>

class PluginDescriptorFile
{
public:
	PluginDescriptorFile(const string& inFile)
	{
		file = string(inFile);
		ifstream iStream(inFile);
		if (iStream.is_open())
		{
			stringstream buffer;
			buffer << iStream.rdbuf();
			originalPluginDescriptorText = buffer.str();
			iStream.close();
		}
	}

	bool IsFileOpen()
	{
		return originalPluginDescriptorText.length() > 0;
	}

	/**
	 *	Will exchange the current file on disk with a new file.
	 *  The content is the same, but the engine version is changed.
	 */
	bool ExchangeEngineVersion(const uint32 majorVersion, const uint32 minorVersion)
	{
		bool bRetVal = false;

		size_t beginLineEngineVersion = originalPluginDescriptorText.find("EngineVersion");
		size_t endLineEngineVersion = originalPluginDescriptorText.find("\n", beginLineEngineVersion);

		string modifiedDescriptorText;
		// If we have found the EngineVersion line
		if (beginLineEngineVersion != string::npos && endLineEngineVersion != string::npos)
		{
			// Start of file until EngineVersion line
			modifiedDescriptorText = originalPluginDescriptorText.substr(0, beginLineEngineVersion - 1);

			// Append modified EngineVersion line
			modifiedDescriptorText.append("\"EngineVersion\": \"").append(to_string(majorVersion)).append(".").append(to_string(minorVersion)).append(".0\",\n");

			// End of file after EngineVersion line
			modifiedDescriptorText.append(originalPluginDescriptorText.substr(endLineEngineVersion + 1));
		}

		if (modifiedDescriptorText.length() > 0)
		{
			ofstream outFile(file, std::ios::trunc);
			if (outFile.is_open())
			{
				outFile << modifiedDescriptorText;
			}
			outFile.close();
		}

		return bRetVal;
	}

	~PluginDescriptorFile()
	{
		// restore original descriptor file
		if (originalPluginDescriptorText.length() > 0)
		{
			ofstream oStream = ofstream(file, std::ios::trunc);
			if (oStream.is_open())
			{
				oStream << originalPluginDescriptorText;
				oStream.close();
			}
		}
	}

	string GetOriginalContent()
	{
		return originalPluginDescriptorText;
	}

private:
	string file;
	string originalPluginDescriptorText;
};
