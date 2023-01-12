@echo off
cd /d %~dp0\engine\premake\
call premake5.exe vs2022
pause