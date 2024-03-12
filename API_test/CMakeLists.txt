cmake_minimum_required(VERSION 3.10)
project(API_demo_project)

set(CMAKE_CXX_STANDARD 11)
set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
set(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 添加链接文件
link_directories(/usr/lib/x86_64-linux-gnu)

file(GLOB SRC_FILES ${SRC}/*.cpp)
file(GLOB INCLUDE_FILES ${INCLUDE_DIRECTORIES}/*.h)
# add_executable(connection main.cpp ${SRC_FILES} ${INCLUDE_FILES})
add_executable(connection ylmain.cpp ${SRC_FILES} ${INCLUDE_FILES})
target_link_libraries(connection mysqlclient)