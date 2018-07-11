cmake_minimum_required(VERSION 3.2) #minimum required version of CMake
project(SolutionName) #set solution name

#Set version number
Set(SolutionName_VERSION_MAJOR 1)
Set(SolutionName_VERSION_MINOR 0)

#Set configure file (purpose : pass some cmake settings to souce code)
configure_file(
	"${PROJECT_SOURCE_DIR}/SolutionNameConfig.h.in"
	"${PROJECT_BINARY_DIR}/SolutionNameConfig.h"
	)

#Set executable solution path 
#set(buildDir ${PROJECT_SOURCE_DIR}/build) TODO : set artefacts directory for the solution
set(EXECUTABLE_OUTPUT_PATH build) #Set output dir

#Set binary output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build") #Set output dir

#Add a static library
include_directories ("${PROJECT_SOURCE_DIR}/LibName") #include lib directory
add_subdirectory (LibName) #call top level CMakeLists.txt

#Add a dynamic library (load time linking)
include_directories("${PROJECT_SOURCE_DIR}/LoadTimeDllName") #include dll directory
add_subdirectory (LoadTimeDllName) #call top level CMakeLists.txt

#Add a dynamic library (run time linking)
include_directories("${PROJECT_SOURCE_DIR}/RunTimeDllName") #include dll directory
add_subdirectory (RunTimeDllName) #call top level CMakeLists.txt

#Add an executable
file(GLOB src *.h *.cpp) # generate a list of all files that match the globbing expressions into src variable

add_executable(ExeName ${src}) #add a .exe project

target_link_libraries(ExeName LibName) #LibName static lib is linked into ExeName

target_link_libraries(ExeName LoadTimeDllName) #LoadTimeDllName dynamic lib is linked into ExeName

#target_link_libraries(ExeName RunTimeDllName) #RunTimeDllName dynamic lib is linked into ExeName