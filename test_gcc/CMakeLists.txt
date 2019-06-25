cmake_minimum_required (VERSION 2.6)
project (test_gcc)
add_executable(test_gcc test.cpp)
target_link_libraries(test_gcc "C:/code/psh_host/x64/Release/psh_host.dll")