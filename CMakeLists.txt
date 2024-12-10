cmake_minimum_required(VERSION 3.13.1)
project(chris-dev-v1 VERSION 1.2.3)
add_executable(app main.cpp)

if(NOT TARGET app)
     message(FATAL_ERROR "Please create a target named 'app' (ex: add_executable(app)) before adding this file")
endif()
set(OpenCV_DIR opencv_4.9.0/build)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include(edge-impulse-sdk/cmake/utils.cmake)
add_subdirectory(edge-impulse-sdk/cmake/zephyr)

RECURSIVE_FIND_FILE_APPEND(MODEL_SOURCE "tflite-model" "*.cpp")
target_include_directories(app PRIVATE .)

# add all sources to the project
target_sources(app PRIVATE ${MODEL_SOURCE})

target_link_libraries(app ${OpenCV_LIBS})