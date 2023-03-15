project(Spectre)

cmake_minimum_required(VERSION 3.0)     # minimum version of CMake
set(CMAKE_CXX_STANDARD 11)              # we need to use C++11
find_package(LLVM REQUIRED CONFIG)      # find the installed LLVM

# print some status messages
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})       # add LLVM's definitions
include_directories(${LLVM_INCLUDE_DIRS})  # include LLVM's development headers

# change this directory to the subfolder of your pass
add_subdirectory(spectre)