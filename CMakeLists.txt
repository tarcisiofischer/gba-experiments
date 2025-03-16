cmake_minimum_required(VERSION 3.16)
project(GBADev)

enable_language(ASM)
set(CMAKE_ASM_COMPILER "$ENV{GBA_LLVM}/bin/clang")
set(CMAKE_C_COMPILER "$ENV{GBA_LLVM}/bin/clang")
set(CMAKE_CXX_COMPILER "$ENV{GBA_LLVM}/bin/clang++")

add_subdirectory(libgba/)
add_subdirectory(examples/)

