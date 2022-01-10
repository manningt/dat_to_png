cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON)
project(dat2png)

add_compile_options(-Wall)
add_compile_options(-Wextra)

file(GLOB SRC_FILES CONFIGURE_DEPENDS "*.c")
add_executable(${PROJECT_NAME} ${SRC_FILES}) 

set(TARGET_NAME ${PROJECT_NAME})
 
target_link_libraries(${PROJECT_NAME} m png)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.out")
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "/home/pi/boomer/staged")
