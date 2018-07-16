@echo off

set MAKEFILE=%~dp0makefile_define.mak
echo BUILD_PBP => "%MAKEFILE%"

set HEADERFILE=%~dp0define.h
echo //#define RUN_FROM_GAME> "%HEADERFILE%"
echo #ifndef RUN_FROM_GAME>> "%HEADERFILE%"
echo #define PRX>> "%HEADERFILE%"
echo #endif>> "%HEADERFILE%"
