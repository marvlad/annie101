cmake_minimum_required(VERSION 3.10)
project(BoostStore)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(GCC_Flags_For_CXX "-Wall -fPIC -pthread -Wformat=0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_Flags_For_CXX}")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")

# Set C++ compiler
set(CMAKE_CXX_COMPILER g++)

# Find Boost
find_package(Boost 1.89.0 REQUIRED COMPONENTS date_time serialization iostreams)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "Boost not found. Please ensure Boost is installed.")
endif()

# Find ROOT package (not required)
find_package(ROOT COMPONENTS Hist)
if(ROOT_FOUND)
    add_definitions(-DHAVE_ROOT) # Define HAVE_ROOT only if ROOT is found
    include_directories(${ROOT_INCLUDE_DIRS})
    link_directories(${ROOT_LIBRARY_DIRS})
else()
    message(WARNING "ROOT not found. ROOT-dependent features will be disabled.")
endif()

# Include directories for ZeroMQ
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../../Tools
)

# Source files
set(SOURCES
    BoostStore101.cpp
    ${CMAKE_SOURCE_DIR}/../../Tools/BoostStore.cpp
    src/MRDOut.cpp
)

# Target executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Boost::date_time
    Boost::serialization
    Boost::iostreams
)
if(ROOT_FOUND)
    target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES})
endif()

# Clean target (optional, CMake handles this differently)
set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES
    "*.bin;*.data;*.txt;*.root;src/*.o"
)
