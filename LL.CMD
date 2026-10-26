@echo off
if "%1"=="" goto usage

if exist %1.def goto with_def

link /NOI /NOD %1.obj, %1.exe, NUL, C:\C600\LIB\SLIBC7.LIB OS2.LIB;
goto end

:with_def
link /NOI /NOD %1.obj, %1.exe, NUL, C:\C600\LIB\SLIBC7.LIB OS2.LIB, %1.def;
goto end

:usage
echo Usage: LL filename (without .OBJ or .EXE extension)

:end