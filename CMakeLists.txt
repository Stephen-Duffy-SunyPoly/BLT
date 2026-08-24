cmake_minimum_required(VERSION 4.0)
project(betterLanguageForTeenyAT)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

set(ABSL_BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Fetch Abseil first because RE2 depends on it.
FetchContent_Declare(
        absl
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
        GIT_TAG        20250512.1
        OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(absl)

# Don't build RE2's tests.
set(RE2_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(RE2_INSTALL OFF CACHE BOOL "" FORCE)

# Regex lib
FetchContent_Declare(
        re2
        GIT_REPOSITORY https://github.com/google/re2.git
        GIT_TAG 2025-11-05
)

FetchContent_MakeAvailable(re2)

add_executable(betterLanguageForTeenyAT main.cpp
        lexer.cpp
        lexer.hpp
        ast_base.cpp
        ast_base.hpp
        DataTypes.hpp)
target_link_libraries(betterLanguageForTeenyAT PRIVATE re2::re2)
