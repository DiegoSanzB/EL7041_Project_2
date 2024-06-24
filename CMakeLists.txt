cmake_minimum_required(VERSION 3.25.0)
project(Project_2_EL7401 
        VERSION 0.1.0 
        DESCRIPTION "EL7041 Project 2"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find ALGLIB library
find_library(ALGLIB_LIBRARY NAMES alglib PATHS /usr/lib/x86_64-linux-gnu)

if(NOT ALGLIB_LIBRARY)
    message(FATAL_ERROR "ALGLIB library not found")
endif()

# Create imported target for ALGLIB
add_library(ALGLIB::alglib UNKNOWN IMPORTED)
set_target_properties(ALGLIB::alglib PROPERTIES
    IMPORTED_LOCATION "${ALGLIB_LIBRARY}"
)

# Add your executable
add_executable(Project_2_EL7401 
    main.cpp
    src/sequence_generator.cpp
    src/data_manager.cpp
)

# Link ALGLIB to your target
target_link_libraries(Project_2_EL7401 PRIVATE ALGLIB::alglib)

# Include ALGLIB headers
# Assuming the ALGLIB headers are in /usr/include or a system path
target_include_directories(Project_2_EL7401 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
