debug:
    msbuild EU4ConsolePatcher.sln /t:Rebuild /p:Configuration=Debug
    
release:
    msbuild EU4ConsolePatcher.sln /t:Rebuild /p:Configuration=Release