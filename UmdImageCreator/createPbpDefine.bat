@echo off

set MAKEFILE=%~dp0makefile_define.mak
echo BUILD_PBP = ^1> "%MAKEFILE%"
echo PSP_FW_VERSION = 661>> "%MAKEFILE%"

set HEADERFILE=%~dp0define.h
echo #define RUN_FROM_GAME> "%HEADERFILE%"
echo #ifdef RUN_FROM_GAME>> "%HEADERFILE%"
echo #define PBP>> "%HEADERFILE%"
echo #endif>> "%HEADERFILE%"
