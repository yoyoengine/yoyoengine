cmake_minimum_required(VERSION 3.22.1)
project(yeptest)

include_directories(include ${CMAKE_SOURCE_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

set(SOURCES
    test.c
    yep.c
)

add_executable(yeptest ${SOURCES})
# target_link_libraries(${EXECUTABLE_NAME} PRIVATE yoyoengine m)