cmake_minimum_required(VERSION 2.5)
project(connectionpool)


set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

aux_source_directory(. SRC_LIST)

add_executable(connectionPool ${SRC_LIST})

target_link_libraries(connectionPool pthread mysqlclient)
