cmake_minimum_required (VERSION 2.6)
project (test_gcc)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
add_executable(test_gcc test.cpp)

get_filename_component(PARENT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)


include_directories(${PARENT_DIR})

target_link_libraries(test_gcc ${PARENT_DIR}/x64/Release/psh_host.dll)