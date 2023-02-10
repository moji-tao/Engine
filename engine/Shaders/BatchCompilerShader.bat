@echo off
cd /d %~dp0\..\..\bin
call HLSLCompiler.exe -j ../engine/Shaders/src/batchShader.json