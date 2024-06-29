cmake_minimum_required(VERSION 3.25.0)
project(Project_2_EL7401 
        VERSION 0.1.0 
        DESCRIPTION "EL7041 Project 2"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/static_libs")

include(FetchContent)
FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(tomlplusplus)

# FetchContent_Declare(
#         libalglib
#         GIT_REPOSITORY https://github.com/S-Dafarra/alglib-cmake.git
# )

# FetchContent_MakeAvailable(libalglib)

# FetchContent_GetProperties(tomlplusplus)
# if(NOT tomlplusplus_POPULATED)
#     FetchContent_Populate(tomlplusplus)
#     add_subdirectory(${qamdemod_SOURCE_DIR} ${tomlplusplus_BINARY_DIR}) # add this line
# endif()



# include_directories(${tomlplusplus_INCLUDE_DIRS})
# add_subdirectory(${tomlplusplus_SOURCE_DIR} ${tomlplusplus_BINARY_DIR})

file(GLOB libalglib_SRC
        "alglib-cpp/src/*.cpp")

add_executable(Project_2_EL7401 main.cpp
                src/sequence_generator.cpp
                src/data_manager.cpp
                src/channel_generator.cpp
                src/demodulation.cpp
                src/interpolations.cpp
                src/BER.cpp
                src/questions.cpp
                QAMDemodulation/QAM\ Demodulation.cpp
                ${libalglib_SRC}
)

target_include_directories(Project_2_EL7401 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Project_2_EL7401 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/QAMDemodulation)
target_include_directories(Project_2_EL7401 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/alglib-cpp/src)

# add_library(libalglib STATIC)

target_link_libraries(Project_2_EL7401 tomlplusplus::tomlplusplus)