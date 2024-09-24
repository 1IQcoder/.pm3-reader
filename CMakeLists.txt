cmake_minimum_required(VERSION 3.3.2)
project(MyApp C CXX)

include(cmake/App.cmake)

set(SOURCES "src/MyApp.h"
            "src/MyApp.cpp"
            "src/main.cpp"
            "src/pm3-reader/file_func.h"
            "src/pm3-reader/file_func.cpp"
            "src/pm3-reader/bmp_preview.cpp"
            "src/pm3-reader/bmp_preview.h")

add_app("${SOURCES}")