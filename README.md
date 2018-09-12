# EU4ConsolePatcher
A simple memory patcher which enables the internal developer console in ironman mode

## Download
* Latest pre-compiled binary^releases can be found in the [release secion](https://github.com/steeno/EU4ConsolePatcher/releases)

## Todo
* Rework Memory/Patch classes

## Latest supported EU4 version
* 1.26.0 (Mughals/Dharma) 
* 1.26.1 Opt-in Hotfix
* Should work with newer versions too

## Build instructions
### Windows (ui)
* Clone repository
* Open `EU4ConsolePatcher.sln` in your Visual Studio IDE
* Choose either `Debug` or `Release` configuration
* Choose either `x86` or `x64` platform 
* Run build
### Windows (cmdline)
* Start cmdline
* Execute `vcvarsall.bat` [arch]
  * e.g. `vcvarsall.bat x64`
  * `vcvarsall.bat` is f.e. located at `X:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\` (VS Community 2017)
* Navigate to EU4ConsolerPatcher project folder
* Run `nmake release` or `nmake debug`

## Usage
* Start Europa Universalis 4
* Start a new ironman game or load a previously saved game
* As soon as the game has loaded, start `EU4ConsolePatcher.exe`
* Open the ingame console with `^` and enter your commands, e.g. `help`
* Have fun
* Sample video: https://youtu.be/BMrR6EO4OAw
