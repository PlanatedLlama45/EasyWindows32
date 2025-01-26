cmake_minimum_required(VERSION 3.13.0)
project(EasyWindows32 VERSION 1.0.0)

set(ENABLE_CONSOLE FALSE)

file(
    GLOB SOURCES

    src/*.c
    src/*.h
    src/*.cpp
    src/*.hpp
)

if (ENABLE_CONSOLE)
    add_executable(${PROJECT_NAME} ${SOURCES})
else()
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "example")
