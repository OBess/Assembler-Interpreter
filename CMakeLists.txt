cmake_minimum_required(VERSION 3.22)

project(AssemblerCompiler)

file(GLOB src *.cpp)
add_executable(AssemblerCompiler ${src})