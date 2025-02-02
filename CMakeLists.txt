cmake_minimum_required(VERSION 3.13.0)
project(EasyWindows32 VERSION 1.0.0)

set(ENABLE_CONSOLE TRUE)
set(EXAMPLE_ID 2)

file(
    GLOB SOURCES

    example/EasyWindows32.hpp
    example/example${EXAMPLE_ID}.cpp
)

file(COPY_FILE example/${PROJECT_NAME}.hpp ${PROJECT_NAME}.hpp)

if (ENABLE_CONSOLE)
    add_executable(${PROJECT_NAME} ${SOURCES})
else()
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "example${EXAMPLE_ID}")
