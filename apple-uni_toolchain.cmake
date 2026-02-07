set(CMAKE_SYSTEM_NAME Darwin)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_C_FLAGS "-arch x86_64 -arch arm64")
set(CMAKE_CXX_FLAGS "-arch x86_64 -arch arm64")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
