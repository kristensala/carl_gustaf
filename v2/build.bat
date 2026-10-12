@echo off
setlocal
pushd "%~dp0"

if errorlevel 1 exit /b 1

rem Uncomment if GCC/G++ are not already on PATH:
rem set "PATH=C:\msys64\ucrt64\bin;%PATH%"

if not exist "build" mkdir "build"
if not exist "build" goto :build_failed

g++ -std=c++20 -O2 -flto -shared -DWINAPI_BUILD -I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.28000.0\cppwinrt" -o ".\build\winapi.dll" ".\lib\winapi.cpp" "-Wl,--out-implib,./build/libwinapi.dll.a" -lruntimeobject -lole32 -loleaut32

if errorlevel 1 goto :build_failed

rem -mwindows so it would not be a console application
gcc -O2 -flto main.c -I ".\lib" -L ".\build" -lwinapi -o ".\build\launcher.exe" -luser32 -lgdi32 -lole32

if errorlevel 1 goto :build_failed

echo Build successful: build\launcher.exe
echo Launching application...

.\build\launcher.exe

if errorlevel 1 goto :exec_failed

popd
exit /b 0

:build_failed
echo Build failed.
popd
exit /b 1

:exec_failed
echo Execution failed.
popd
exit /b 1
