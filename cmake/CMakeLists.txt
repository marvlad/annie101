cmake_minimum_required(VERSION 3.10) 

# Set the project name
project(HelloWorldCMake)

# Let;s get the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable, what to do?
add_executable(helloFromCMake mycode.cpp)
