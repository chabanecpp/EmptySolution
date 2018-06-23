cmake_minimum_required(VERSION 3.2) #minimum required version of CMake
project(SolutionName) #set solution name

include_directories ("${PROJECT_SOURCE_DIR}/LibName") #include lib
add_subdirectory (LibName) #call top level CMakeLists.txt
 
add_executable(ExeName main.cpp) #add a .exe project

target_link_libraries(ExeName LibName) #LibName static lib is linked into ExeName