cmake_minimum_required (VERSION 3.20)
project (Pass CXX)

set(CMAKE_CXX_COMPILER clang++)

add_library (${PROJECT_NAME}
    SHARED
        source/LLVMPass.cpp
)

target_include_directories (${PROJECT_NAME}
    PRIVATE

)

target_compile_features (${PROJECT_NAME}
    PRIVATE
        cxx_std_17
)

target_compile_options (${PROJECT_NAME}
    PRIVATE
        -fdiagnostics-color=always

        -I /usr/lib/llvm-18/include

        -Wall
        -Wextra
        -fstack-protector-strong
        -fcheck-new
        -fstrict-overflow

        -ldl

        -rdynamic

        -march=native
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

target_link_options (${PROJECT_NAME}
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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # to generate compile_commands.json

# cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
# cmake --build build
