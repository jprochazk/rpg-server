@echo off
pushd %~dp0\..\
call conan install . -s build_type=%1
call premake5.exe %2
popd