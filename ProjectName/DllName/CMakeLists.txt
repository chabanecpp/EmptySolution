file(GLOB src *.h *.cpp) # generate a list of all files that match the globbing expressions into src variable

add_library(DllName SHARED ${src}) #Add a .dll library