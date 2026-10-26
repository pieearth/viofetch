@echo off
if "%1"=="" goto usage

cl -c -AS -G2 -Zp -W3 %1.c
goto end

:usage
echo Usage: CC filename (without .C extension)

:end