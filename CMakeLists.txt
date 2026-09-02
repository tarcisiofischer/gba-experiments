cmake_minimum_required(VERSION 3.16)
project(GBADev)

# Compilers are configured via the GBA toolchain file.
# Configure with: cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/libgba/cmake/gba-toolchain.cmake

find_package(libgba REQUIRED
  PATHS ${LIBGBA_PATH}
  NO_DEFAULT_PATH
)

add_subdirectory(examples/)

