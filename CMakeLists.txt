cmake_minimum_required(VERSION 3.10)

project(pm3-reader)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(myfiles "devfiles")

# find_package(fmt CONFIG REQUIRED)

add_subdirectory(OpenXLSX)
add_executable(${PROJECT_NAME} ${myfiles}/main.cpp ${myfiles}/image_utils.cpp ${myfiles}/exel_func.cpp ${myfiles}/bytes_func.cpp)

target_link_libraries(${PROJECT_NAME} OpenXLSX::OpenXLSX)

set(BAT_FILE_PATH "${CMAKE_SOURCE_DIR}/flagsremover.bat")

add_custom_target(run_prepare
    COMMAND cmd /c ${BAT_FILE_PATH} "${PROJECT_NAME}" "/utf-8"
    COMMENT "Starting ${BAT_FILE_PATH}..."
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_dependencies(${PROJECT_NAME} run_prepare)