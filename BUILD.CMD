@echo off
if "%1"=="" goto usage

call cc %1
if errorlevel 1 goto failed

call ll %1
if errorlevel 1 goto failed

echo Build succeeded for %1.exe
goto end

:failed
echo Build failed for %1.

:usage
echo Usage: BUILD filename (without extension)

:end