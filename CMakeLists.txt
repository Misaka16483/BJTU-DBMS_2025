cmake_minimum_required(VERSION 3.10.0)
project(dbms VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(dbms main.cpp src/Table.cpp src/Data.cpp src/Database.cpp src/exec.cpp src/File.cpp)

target_link_libraries(dbms PRIVATE libparser.a)