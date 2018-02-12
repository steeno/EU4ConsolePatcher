# EU4ConsolePatcher
A simple memory patcher which enables the internal developer console in ironman/multiplayer mode

## Todo
* Rework Memory/Patch class
* Use pattern scanning instead of fixed RVA's

## Latest supported EU4 version
* 1.24.1 (Japan Patch)

## Build instructions
### Windows (ui)
* Clone repository
* Open EU4ConsolePatcher.sln in Visual Studio
* Choose either Debug or Release build
* Choose either x86 or x64 build
* Build the project
### Windows (cmdline)
* Start cmdline
* Execute `vcvarsall.bat` [arch]
  * e.g. `vcvarsall.bat x64`
* Navigate to project folder
* Run `MSBuild EU4ConsolePatcher.sln /t:Rebuild /p:Configuration=Release`

## Usage
* Start Europa Universalis 4
* Start a new ironman game or load a previously saved game
* As soon as the game has loaded, start `EU4ConsolePatcher.exe`
* Open the ingame console with `^` and enter your commands, e.g. `help`
* Have fun
* Sample video: https://youtu.be/BMrR6EO4OAw

## Issues
* v1.0
  * If you get an error like "msvcp140.dll not found" while trying to use the binary release you would need to install the "Visual C++ Redistributable Package for VS2017" first, which can be found [here](https://go.microsoft.com/fwlink/?LinkId=746572). This should be solved in future releases.
