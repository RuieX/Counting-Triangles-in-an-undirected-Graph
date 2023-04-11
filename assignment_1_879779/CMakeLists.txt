cmake_minimum_required(VERSION 3.24)
project(learning_massive_data)

# Enable OpenMP support
find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

add_executable(main "assignments/asgmt_1/main.cpp" assignments/asgmt_1/Graph_ds.cpp assignments/asgmt_1/Graph_ds.h)
target_link_libraries(main PRIVATE OpenMP::OpenMP_CXX)
