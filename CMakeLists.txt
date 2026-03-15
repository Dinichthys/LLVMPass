cmake_minimum_required(VERSION 3.20)
project(Pass CXX C)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/log)

# ---------------------------------------------------
#
# LLVMPass
#
# ---------------------------------------------------

add_library(${PROJECT_NAME}
    SHARED
        source/pass/llvm_pass.cpp
        source/pass/static_info.cpp
        source/pass/dynamic_info.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        include
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -fdiagnostics-color=always

        -I /usr/lib/llvm-18/include

        -Wall
        # -Wextra
        -fstack-protector-strong
        -fcheck-new
        -fstrict-overflow

        $<$<CONFIG:Debug>:
            -Og
            # -fno-elide-constructors
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -fPIC
            # -flto=auto
            -DNDEBUG
        >
)

target_link_options(${PROJECT_NAME}
    PRIVATE
        -march=native
        -Wl,-q
        $<$<CONFIG:Debug>:
            -Og
            # -fno-elide-constructors
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -fPIC
            -flto=auto
        >
)

# ---------------------------------------------------
#
# Logging functions for dynamic info
#
# ---------------------------------------------------

set(LOG_FUNC_OBJ log_func)

add_library(${LOG_FUNC_OBJ}
    OBJECT
        source/pass/log_func.c
)

target_include_directories(${LOG_FUNC_OBJ}
    PRIVATE
        include
)

target_compile_options(${LOG_FUNC_OBJ}
    PRIVATE
        -fdiagnostics-color=always

        -Wall
        -Wextra
        -fstack-protector-strong
        -fcheck-new
        -fstrict-overflow

        $<$<CONFIG:Debug>:
            -Og
            # -fno-elide-constructors
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -fPIC
            # -flto=auto
            -DNDEBUG
        >
)

target_link_options(${LOG_FUNC_OBJ}
    PRIVATE
        -march=native
        -Wl,-q
        $<$<CONFIG:Debug>:
            -Og
            # -fno-elide-constructors
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -fPIC
            -flto=auto
        >
)

# ---------------------------------------------------
#
# Reading info and building graph
#
# ---------------------------------------------------

set(BUILD_GRAPH build_graph)

add_executable(${BUILD_GRAPH}
    source/graph/graph.cpp

    source/graph/read/read_static_info.cpp
    source/graph/read/read_dynamic_info.cpp

    source/graph/write/write_graph.cpp
)

target_include_directories(${BUILD_GRAPH}
    PRIVATE
        include
)

target_compile_options(${BUILD_GRAPH}
    PRIVATE
        -fdiagnostics-color=always

        -I /usr/lib/llvm-18/include

        -Wall
        # -Wextra
        -fstack-protector-strong
        -fcheck-new
        -fstrict-overflow

        $<$<CONFIG:Debug>:
            -Og
            # -fno-elide-constructors
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -fPIC
            # -flto=auto
            -DNDEBUG
        >
)

target_link_options(${BUILD_GRAPH}
    PRIVATE
        -march=native
        -Wl,-q
        $<$<CONFIG:Debug>:
            -Og
            # -fno-elide-constructors
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -fPIC
            -flto=auto
        >
)

# ---------------------------------------------------
#
# Generating compile_commands.json
#
# ---------------------------------------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
# cmake --build build
