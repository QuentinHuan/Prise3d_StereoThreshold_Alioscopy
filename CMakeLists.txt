cmake_minimum_required(VERSION 3.16.3)
project(MinimalOpenGL)


#headers directory
include_directories(include)

#find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

#find SDL2_ttf
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(SDL2TTF REQUIRED)
include_directories(${SDL2TTF_INCLUDE_DIRS})
link_directories (${SDL2TTF_LIBRARIES})

#find OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

#find GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

#source directory
file(GLOB SOURCES "src/*.c*" "include/*.c*")

#link 
add_executable(main ${SOURCES})
target_link_libraries(main ${SDL2_LIBRARIES})
target_link_libraries(main ${SDL2TTF_LIBRARY})
target_link_libraries(main ${OPENGL_LIBRARIES})
target_link_libraries(main ${GLEW_LIBRARIES})
target_link_libraries(main ${CMAKE_DL_LIBS})
