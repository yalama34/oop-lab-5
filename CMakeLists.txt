cmake_minimum_required(VERSION 3.20)
project(oop_lab_5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_SANITIZERS "Enable AddressSanitizer" OFF)

function(configure_target target)
    target_compile_options(${target} PRIVATE
        -Wall -Wextra -Wpedantic
    )

    if(ENABLE_SANITIZERS)
        target_compile_options(${target} PRIVATE
            -fsanitize=address
            -fno-omit-frame-pointer
        )
        target_link_options(${target} PRIVATE
            -fsanitize=address
        )
    endif()
endfunction()

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(FILTER SOURCES EXCLUDE REGEX "/main\\.cpp$")

add_library(lab5_core STATIC ${SOURCES})
target_include_directories(lab5_core PUBLIC src/include)
configure_target(lab5_core)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE lab5_core)
configure_target(app)

include(CTest)

if(BUILD_TESTING)
    find_package(GTest QUIET)

    if(NOT GTest_FOUND)
        include(FetchContent)

        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
            TLS_VERIFY OFF
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )

        FetchContent_MakeAvailable(googletest)
    endif()

    add_executable(lab5_tests tests/test_tree.cpp)

    target_link_libraries(lab5_tests PRIVATE
        lab5_core
        GTest::gtest_main
    )

    configure_target(lab5_tests)

    include(GoogleTest)
    gtest_discover_tests(lab5_tests DISCOVERY_MODE PRE_TEST)
endif()