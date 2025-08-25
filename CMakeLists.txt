cmake_minimum_required(VERSION 2.6)

project(armsim)

option(BUILD_SHARED_LIBS "Create kernel as shared/dynamic library." ON)
option(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS "Export all non-static symbols on Win32" ON)

add_library(ask ask.cpp decode.cpp execute.cpp)

# Build test program (every time)
add_executable(tests_exe ask.cpp decode.cpp execute.cpp tests.cpp)

# Run test (if so requested)
enable_testing()
add_test(NAME test COMMAND tests_exe)

