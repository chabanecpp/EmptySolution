file(GLOB src *.h *.cpp) # generate a list of all files that match the globbing expressions into src variable

add_library(LoadTimeDllName SHARED ${src}) #Add a .dll library