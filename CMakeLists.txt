cmake_minimum_required(VERSION 3.20)

project(LinuxMonitor LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(linux_monitor 
    src/main.c
    src/cpu.c
    )

target_include_directories(linux_monitor PRIVATE include)

target_compile_options(linux_monitor PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
