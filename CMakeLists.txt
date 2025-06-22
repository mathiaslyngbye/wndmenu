cmake_minimum_required(VERSION 3.15)
project(wndmenu)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DUNICODE -D_UNICODE)

add_executable(wndmenu
    src/main.cpp
)

#set_target_properties(wndwalk PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

