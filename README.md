# UnrealPluginBuilder

Little tool that helps package a unreal plugin for multiple engine versions without the need of adding it to a project. 
The engine versions to build for have to be installed via the UnrealLauncher.

This plugin automatically changes the engine version within the .uplugin file of the plugin to be built. Therefore the .uplugin file must be writeable (might not be cause of SourceControl). After execution, the file is changed back to its previous state.

## Instructions
- Build the executeable (change YOUREPICGAMESDIRECTORY in main.cpp if needed).
- Place the executable where ever you want (The directory name should not be to long, otherwise packaging might not work, subfolder on Desktop is fine).
- Drop a plugin folder (of the plugin to be built) onto the executeable and follow the instructions.
- Packaged plugin will be output next to the executeable, one folder per version.
- Optional marketplace deployment option. Will remove folders like binary/intermediate and zip it up.
