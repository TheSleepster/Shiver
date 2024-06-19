@echo off
REM /Ot /O2 for release
REM remove -Zi

Set CommonCompilerFlags=-DSHIVER_SLOW=1 -DENGINE=1 -nologo -Fm -GR- -EHa- -Od -Oi -Zi -W4 -wd4189 -wd4200 -wd4996 -wd4706 -wd4530 -wd4100 -wd4201 -wd4505
Set CommonLinkerFlags=-incremental:no kernel32.lib user32.lib gdi32.lib opengl32.lib "../data/deps/FMOD/fmodL_vc.lib" "../data/deps/FMOD/fmodstudio_vc.lib" -SUBSYSTEM:WINDOWS -ENTRY:WinMainCRTStartup

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
cl ../code/Win32_Shiver.cpp -MT -I"../data/deps" %CommonCompilerFlags% -link %CommonLinkerFlags% -OUT:"Shiver.exe" 
popd

@echo ====================
@echo Compilation Complete
@echo ====================