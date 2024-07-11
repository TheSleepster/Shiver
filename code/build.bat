@echo off
REM /Ox /O2 /Ot for release
REM -Bt+ for timing info
REM remove -Zi

Set opts=-DSHIVER_SLOW=1 -DENGINE=1 -DFMOD=0

Set CommonCompilerFlags=-Fp:fast -Fm -GR- -EHa- -Od -Oi -Zi -W4 -wd4189 -wd4200 -wd4996 -wd4706 -wd4530 -wd4100 -wd4201 -wd4505
Set CommonLinkerFlags= /NOLOGO -incremental:no kernel32.lib user32.lib gdi32.lib opengl32.lib "../data/deps/MiniAudio/miniaudio.lib"

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
del *.pdb
cl %opts% ../code/Win32_Shiver.cpp -I"../data/deps" %CommonCompilerFlags% -MT -link %CommonLinkerFlags% -OUT:"Shiver.exe" 
cl %opts% ../code/Shiver.cpp -I"../data/deps" %CommonCompilerFlags% -MT -LD -link -incremental:no -PDB:Shiver_%RANDOM%.pdb -EXPORT:GameUnlockedUpdate -EXPORT:GameFixedUpdate -EXPORT:GameOnAwake -OUT:"../data/ShiverGame.dll"

popd

@echo ====================
@echo Compilation Complete
@echo ====================
