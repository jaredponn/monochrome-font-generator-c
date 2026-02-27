cmake_minimum_required(VERSION 3.20)
project(monochrome-font-generator-c LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Freetype REQUIRED)

add_executable(monochrome-font-generator-c src/main.c src/cli.c)
target_link_libraries(monochrome-font-generator-c PRIVATE Freetype::Freetype)
install(TARGETS monochrome-font-generator-c DESTINATION bin)
