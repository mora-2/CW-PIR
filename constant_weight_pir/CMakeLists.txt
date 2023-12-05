cmake_minimum_required(VERSION 3.10)

project (cwpir VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SEAL)
find_package(OpenMP)

set(SOURCE_FILES  PIRClient.cpp PIRServer.cpp utils.cpp database.cpp params.cpp)
file(GLOB HEADERS "*.h")

# add_library(cwpir ${SOURCE_FILES} ${HEADERS})
add_library(cwpir SHARED ${SOURCE_FILES})

set_target_properties(cwpir PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# set_target_properties(cwpir PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(cwpir SEAL::seal OpenMP::OpenMP_CXX)
