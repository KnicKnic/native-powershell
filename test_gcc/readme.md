#setup environment
set CMAKE_C_COMPILER=gcc
set CMAKE_CXX_COMPILER=g++
set CMAKE_CXX_COMPILER_ENV_VAR=CXX
set CMAKE_C_COMPILER_ENV_VAR=CC
set CMAKE_MAKE_PROGRAM=C:\ProgramData\chocolatey\lib\mingw\tools\install\mingw64\bin\nm.exe

#read someone else's tutorial
https://www.scivision.dev/windows-gcc-gfortran-cmake-make-install/


I isntalled gcc through tdd-gcc in chocolatey or maybe it was mingw

cmake -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -B bin .

cmake --build .

cmake --build . --target install   # optional

# this actually worked
cmake -G "MinGW Makefiles" .
mingw32-make.exe

