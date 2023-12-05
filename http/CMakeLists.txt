# 设置最低 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS -g)

project (main VERSION 1.0)

add_executable(main main.cpp)

include_directories("/home/yuance/Work/Encryption/PIR/code/PIR/CWPIR/constant_weight_pir")
add_subdirectory("/home/yuance/Work/Encryption/PIR/code/PIR/CWPIR/constant_weight_pir" pir)
target_link_libraries(main PUBLIC cwpir)