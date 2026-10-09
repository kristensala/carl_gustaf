#!/usr/bin/env bash
#set -e

mkdir -p ./build

g++ -std=c++20 -O2 -shared -DWINAPI_BUILD -I 'C:\Program Files (x86)\Windows Kits\10\Include\10.0.28000.0\cppwinrt' -o ./build/winapi.dll ./lib/winapi.cpp '-Wl,--out-implib,./build/libwinapi.dll.a' -lruntimeobject -lole32 -loleaut32
gcc main.c -I ./lib -L ./build -lwinapi -o ./build/launcher.exe -luser32 -lgdi32 -lole32
