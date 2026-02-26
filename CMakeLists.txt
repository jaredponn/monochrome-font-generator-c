cmake_minimum_required(VERSION 3.20)
project(ttf-to-bw LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Freetype REQUIRED)

add_executable(ttf-to-bw src/main.c src/cli.c)
target_link_libraries(ttf-to-bw PRIVATE Freetype::Freetype)
install(TARGETS ttf-to-bw DESTINATION bin)
