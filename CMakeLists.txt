cmake_minimum_required(VERSION 3.12)

project(UNIT_TEST LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
    ${CMAKE_PROJECT_NAME}
    main.cpp
    VectorStore.cpp
    VectorStore.h
    main.h
    utils.h
)

add_custom_target(
    run
    DEPENDS ${CMAKE_PROJECT_NAME}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}
)

target_compile_options(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -Werror
        -Wno-unused-parameter
        -Wno-deprecated-copy
        -g3
)
