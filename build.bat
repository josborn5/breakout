REM https://youtu.be/IztkFkg2Y-4?t=13024

REM VS 2015:
REM call "%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

REM VS 2017:
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
@echo off

SET CODE_DIR=code
SET OUTPUT_DIR=bin

call "%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
IF NOT EXIST %OUTPUT_DIR% mkdir %OUTPUT_DIR%
pushd %OUTPUT_DIR%

SET COMMON_COMPILER_FLAGS=-MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4201 -DFLAG_SLOW=1 -DFLAG_WIN32=1 -FC -Z7 -Fmwin32_platform.map
SET COMMON_LINKER_FLAGS=-opt:ref user32.lib Gdi32.lib winmm.lib

REM 32-bit build
REM cl %COMMON_COMPILER_FLAGS% ..\code\win32_platform.cpp /link -subsystem:windows,5.1  %COMMON_LINKER_FLAGS%

REM 64-bit build
cl %COMMON_COMPILER_FLAGS% ..\%CODE_DIR%\win32_platform.c /link %COMMON_LINKER_FLAGS%

cl %COMMON_COMPILER_FLAGS% ..\%CODE_DIR%\tests.c
popd
