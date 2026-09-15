cmake_minimum_required(VERSION 3.16)

project(Demo LANGUAGES CXX)

add_executable(demo main.cpp)

set_target_properties(demo PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

find_package(OpenCV 4 REQUIRED)
find_package (Eigen3 REQUIRED NO_MODULE)

target_include_directories(demo PRIVATE
${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(demo PRIVATE
${OpenCV_LIBS}
Eigen3::Eigen
)
