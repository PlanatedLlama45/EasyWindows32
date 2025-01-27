cmake_minimum_required(VERSION 3.13.0)
project(EasyWindows32 VERSION 1.0.0)

set(ENABLE_CONSOLE FALSE)

file(
    GLOB SOURCES

    example/*.c
    example/*.h
    example/*.cpp
    example/*.hpp
)

file(COPY_FILE example/${PROJECT_NAME}.hpp ${PROJECT_NAME}.hpp)

if (ENABLE_CONSOLE)
    add_executable(${PROJECT_NAME} ${SOURCES})
else()
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "example")
