cmake_minimum_required(VERSION 3.2) #minimum required version of CMake
project(SolutionName) #set solution name

#Add a static library
include_directories ("${PROJECT_SOURCE_DIR}/LibName") #include lib directory
add_subdirectory (LibName) #call top level CMakeLists.txt

#Add a dynamic library
include_directories("${PROJECT_SOURCE_DIR}/DllName") #include dll directory
add_subdirectory (DllName) #call top level CMakeLists.txt

#Add an executable
file(GLOB src *.h *.cpp) # generate a list of all files that match the globbing expressions into src variable

add_executable(ExeName ${src}) #add a .exe project

target_link_libraries(ExeName LibName) #LibName static lib is linked into ExeName

target_link_libraries(ExeName DllName) #DllName dynamic lib is linked into ExeName