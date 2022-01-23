project(demo)

set(CMAKE_CXX_STANDRAND 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /source-charset:utf-8")

add_executable(demo demo.cpp)