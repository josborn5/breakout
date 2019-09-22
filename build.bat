REM https://youtu.be/IztkFkg2Y-4?t=1037

call "%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

SET CODE_DIR=code
SET OUTPUT_DIR=bin

IF NOT EXIST %OUTPUT_DIR% mkdir %OUTPUT_DIR%

pushd %OUTPUT_DIR%

del *.pdb > NUL 2> NUL

cl -nologo -Zi -FC ..\%CODE_DIR%\win32_platform.c /link user32.lib gdi32.lib -incremental:no -opt:ref

popd