@echo off

pushd "..\build"
g++ -std=c++17 -o tis-100-emulator "..\src\*.cpp" -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
popd
