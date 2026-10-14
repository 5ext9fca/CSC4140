cmake_minimum_required(VERSION 3.16)

project(Ass2 LANGUAGES CXX)

add_executable(ass2
    main.cpp
    config.cpp
    rasterizer.cpp
    Triangle.cpp
)

set_target_properties(ass2 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

find_package(OpenCV 4 REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)

target_include_directories(ass2 PRIVATE
${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(ass2 PRIVATE
${OpenCV_LIBS}
Eigen3::Eigen
)
