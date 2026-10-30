cmake_minimum_required(VERSION 3.20)

project(LinuxMonitor LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# main.
add_executable(linux_monitor 
    src/main.c
    src/cpu.c
    )

target_include_directories(linux_monitor PRIVATE include)

# testing.

add_library(unity STATIC external/Unity/src/unity.c)

target_include_directories(unity PUBLIC external/Unity/src)

enable_testing()

add_executable(test_cpu
    tests/test_cpu.c
    src/cpu.c
)

target_include_directories(test_cpu PRIVATE include)

target_link_libraries(test_cpu PRIVATE unity)

add_test(NAME cpu_tests COMMAND test_cpu)

# compiler options.

target_compile_options(linux_monitor PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
