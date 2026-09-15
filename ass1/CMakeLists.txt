cmake_minimum_required(VERSION 3.16)

project(Demo LANGUAGES CXX)

find_package(OpenCV 4 REQUIRED)

add_executable(demo main.cpp)

target_include_directories(demo PRIVATE
${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(demo PRIVATE
${OpenCV_LIBS}
)

set_target_properties(demo PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)
