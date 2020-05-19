@echo off
pushd %~dp0\..\
call conan install . -s build_type=%1
call premake5.exe --file=%2 %3
popd