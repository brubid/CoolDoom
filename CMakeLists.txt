
cmake_minimum_required (VERSION 3.11)

project (CustomDoomPort)

file(GLOB_RECURSE src "src/*.c")
#Exclude main from src
list(FILTER src EXCLUDE REGEX "src/i_main.c")

add_executable(CustomDoom ${src} src/i_main.c)

target_include_directories(CustomDoom PRIVATE src)


set(SDL3_DIR "" CACHE PATH "SDL3 CMAKE Directory")
list(APPEND CMAKE_PREFIX_PATH SDL3_DIR)
find_package(SDL3 REQUIRED SDL3-shared)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
target_include_directories(CustomDoom PRIVATE ${SDL3_DIR}/include)

target_link_libraries(CustomDoom PRIVATE SDL3::SDL3-shared)

