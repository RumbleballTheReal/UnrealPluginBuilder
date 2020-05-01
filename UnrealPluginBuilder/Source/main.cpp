
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "BasicTypes.h"
#include "Filesystem.h"
#include "Stringhelper.h"
#include "System.h"
#include "PluginDescriptorFile.h"

using namespace std;


#define YOUREPICGAMESDIRECTORY "F:\\Program Files\\EpicGames" 


enum ERetVal
{
	RT_Success = 0,
	RT_WrongUsage = 1,
	RT_WrongVersion = 2,
	RT_FileError = 3,
};

void PrintUseageNote()
{
	cout << "Usage: " << endl;
	cout << "This little program is to build an unreal plugin for distribution (forum/marketplace/...)." << endl;
	cout << "It supports building for multiple Unreal Engine versions at once. The resulting files are output next to the executable, a folder per engine version." << endl;
	cout << "Just drop the directory of the plugin on the executable and follow the instructions." << endl;
	cout << endl;
}


string GetPluginNameFromDirectory(const string& pluginDir)
{
	size_t slashDelimiter = pluginDir.rfind("\\");
	++slashDelimiter;
	return pluginDir.substr(slashDelimiter);
}

string BuildPluginDescriptorFilePath(const string& pluginDir, const string& pluginName)
{
	string out = pluginDir;
	out.append("\\").append(pluginName).append(".uplugin");
	return out;
}


int main(int argCount, char** args)
{
	//cout << "Guten Morgen!" << endl;
	//if (argCount > 0)
	//{
	//	cout << "ProgrammName: " << args[0] << endl;
	//}

	if (argCount <= 1)
	{
		PrintUseageNote();
		System::Pause();
		return ERetVal::RT_WrongUsage;
	}

	string inString;
	string pluginDir = args[1];

	cout << "Plugin to build: " << pluginDir << endl << endl;

	// check for plugin file
	PluginDescriptorFile pluginDescriptorFile(BuildPluginDescriptorFilePath(pluginDir, GetPluginNameFromDirectory(pluginDir)));
	if (!pluginDescriptorFile.IsFileOpen())
	{
		cout << "Could not open plugin descriptor file! Filename: " << pluginDescriptorFile.GetFile() << endl;
		cout << "Is the file write protected?" << endl;
		System::Pause();
		return ERetVal::RT_FileError;
	}
	else
	{
		cout << "Plugin descriptor file found and opened." << endl;
		
		cout << pluginDescriptorFile.GetOriginalContent() << endl << endl;
	}

	cout << "Enter UnrealEngine Major version: ";
	getline(cin, inString);
	uint32 majorVersion = 0;
	stringstream(inString) >> majorVersion;
	if (majorVersion != 4)
	{
		cout << "Currently only Unreal Engine Major Version 4 is supported" << endl;
		System::Pause();
		return ERetVal::RT_WrongVersion;
	}

	cout << "Enter UnrealEngine Minor versions as comma separated, e.g. 10,11,12: ";
	getline(cin, inString);
	vector<string> minorVersionsAsString;
	ParseToArray(inString, ",", minorVersionsAsString);

	cout << "Ready for Marketplace deployment (Remove Binaries/Intermediate and zip it up)? (y/n): ";
	getline(cin, inString);
	if (!(Equal(inString, string("y")) || Equal(inString, string("n"))))
	{
		cout << "Invalid input. Aborting";
		System::Pause();
	}
	bool bReadyForMarketplace = Equal(inString, string("y"));

	if (minorVersionsAsString.size() == 0)
	{
		cout << "No minor versions specified!" << endl;
		System::Pause();
		return ERetVal::RT_WrongVersion;
	}

	vector<uint32> minorVersions;
	for (const string& s : minorVersionsAsString)
	{
		uint32 val;
		stringstream(s) >> val;
		minorVersions.push_back(val);
	}

	cout << "Versions to build for: " << endl;
	for (const uint32& minor : minorVersions)
	{
		cout << "UE" << majorVersion << "." << minor << endl;
	}

	string workingDirectory;
	{
		vector<string> temp;
		ParseToArray(args[0], ".", temp);
		workingDirectory = temp[0];
	}

	// Create a intermediate plugin copy to not modify the source
	string intermediateCopy;
	intermediateCopy.append(workingDirectory).append("\\IntermediateCopy");
	// Make sure no existing intermediate exists
	cout << "Clearing old intermediate copy" << endl;
	System::Exec("rmdir", intermediateCopy + " /s /q");
	cout << "Creating new intermediate copy" << endl;
	System::Exec("robocopy", pluginDir + " " + intermediateCopy + " /s");

	for (const uint32& minor : minorVersions)
	{
		cout << endl << endl << "Building Plugin for version UE" << majorVersion << "." << minor << endl;
		
		string runUATbatch;
		runUATbatch.append(YOUREPICGAMESDIRECTORY).append("\\UE_").append(to_string(majorVersion)).append(".").append(to_string(minor));
		runUATbatch.append("\\Engine\\Build\\BatchFiles\\RunUAT.bat");

		if(FileExists(runUATbatch))
		{
			pluginDescriptorFile.ExchangeEngineVersion(majorVersion, minor);
			pluginDescriptorFile.WriteModifiedFileToDirectory(intermediateCopy);

			string outputDir;
			outputDir.append(workingDirectory).append("\\").append(GetPluginNameFromDirectory(pluginDir)).append(to_string(majorVersion)).append(to_string(minor));
			string arguments;
			arguments.append("BuildPlugin -Plugin=\"").append(BuildPluginDescriptorFilePath(intermediateCopy, GetPluginNameFromDirectory(pluginDir))).append("\" -package=\"").append(outputDir).append("\"").append(" -rocket");

			cout << "Deleting old output directory " << outputDir << endl;
			System::Exec("rmdir", outputDir + " /s /q");

			cout << "Command: " << runUATbatch << endl;
			cout << "Arguments: " << arguments << endl;

			int execRet = 0;
			execRet = System::Exec(runUATbatch, arguments);
			cout << "Process returned with " << execRet << endl;
			
			if (execRet == 0)
			{
				// Copy over files next to the .uplugin file (can be licenses, changelogs, ...)
				// Note: /XO options prevents copy of older files. That is the case for .uplugin file
				System::Exec("robocopy", pluginDir + " " + outputDir + " /XO");

				// In case the build succeeded
				if (bReadyForMarketplace)
				{
					cout << "Creating zipup copy for marketplace\n";

					// copy the output
					string copyDir;
					copyDir.append(workingDirectory).append("\\").append(GetPluginNameFromDirectory(pluginDir));
					System::Exec("robocopy", outputDir + " " + copyDir + " /s");

					// Remove binaries and intermediate directory from copied version
					string rmdirCommand("rmdir");
					string binariesDir = copyDir + ("\\Binaries");
					string intermediateDir = copyDir + ("\\Intermediate");
					System::Exec(rmdirCommand, binariesDir + " /s /q");
					System::Exec(rmdirCommand, intermediateDir + " /s /q");

					// zip it up
					System::Exec("powershell", "\"Compress-Archive -Path " + copyDir + " -DestinationPath " + outputDir + ".zip\"");

					cout << "Deleting zipup copy for marketplace\n";

					// clear the copy
					System::Exec(rmdirCommand, copyDir + " /s /q");
				}
			}
			else
			{
				// In case the build failed
				// Remove the output folder for the specific version
				cout << "Build failed. Remove the output folder for the current version?" << endl;
				string rmdirCommand("rmdir");
				System::Exec(rmdirCommand, outputDir + " /s");
			}
		}
		else
		{
			cout << "Could not find runUAT.bat file! File: " << runUATbatch << endl;
			System::Pause();
		}
	}

	cout << "Removing intermediate copy" << endl;
	System::Exec("rmdir", intermediateCopy + " /s /q");
	
	System::Pause();
	return ERetVal::RT_Success;
}