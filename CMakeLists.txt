cmake_minimum_required(VERSION 3.13.0)
project(xArm VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
#add_compile_definitions(USE_XARM)

include_directories(${PROJECT_NAME} 
        /usr/include
        ${CMAKE_SOURCE_DIR}/xArm-CPLUS-SDK/include
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME} src/main.cpp src/xArm.cpp src/xArm.h include/defines.h src/thread.h)
target_link_libraries(${PROJECT_NAME} -lpthread -lxarm)