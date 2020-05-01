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
        size_t beginLineEngineVersion = originalPluginDescriptorText.find("EngineVersion");
        size_t endLineEngineVersion = originalPluginDescriptorText.find("\n", beginLineEngineVersion);

        if (beginLineEngineVersion == endLineEngineVersion)
        {
            cout << "Plugin descriptor file does not contain line: \"EngineVersion\": \"x.y\"" << endl;
            System::Pause();
            return false;
        }

        modifiedPluginDescriptorText.clear();
        // If we have found the EngineVersion line
        if (beginLineEngineVersion != string::npos && endLineEngineVersion != string::npos)
        {
            // Start of file until EngineVersion line
            modifiedPluginDescriptorText = originalPluginDescriptorText.substr(0, beginLineEngineVersion - 1);

            // Append modified EngineVersion line
            modifiedPluginDescriptorText.append("\"EngineVersion\": \"").append(to_string(majorVersion)).append(".").append(to_string(minorVersion)).append(".0\",\n");

            // End of file after EngineVersion line
            modifiedPluginDescriptorText.append(originalPluginDescriptorText.substr(endLineEngineVersion + 1));
        }

        return true;
    }

    bool WriteModifiedFileToDirectory(string dir)
    {
        if (dir.length() == 0)
        {
            return false;
        }

        if (modifiedPluginDescriptorText.length() > 0)
        {
			string outFile = dir.append("\\").append(GetFileNameAndExtension());
			
			// Delete existing
			System::Exec("del", "/f " + outFile);

			// Write new
            ofstream oStream = ofstream(outFile, std::ios::trunc);
            if (oStream.is_open())
            {
                oStream << modifiedPluginDescriptorText;
                oStream.close();
				return true;
            }
        }

        return false;
    }

    ~PluginDescriptorFile()
    {
    }

    string GetOriginalContent()
    {
        return originalPluginDescriptorText;
    }

	string GetFileNameAndExtension()
	{
		string name;
		size_t slashDelimiter = file.rfind("\\");
		++slashDelimiter;
		return file.substr(slashDelimiter);
	}

	string GetFile()
	{
		return file;
	}

private:
    string file;
    string originalPluginDescriptorText;
    string modifiedPluginDescriptorText;
};
