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
