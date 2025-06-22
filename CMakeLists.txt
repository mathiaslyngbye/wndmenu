cmake_minimum_required(VERSION 3.15)
project(wndmenu)

# Attempt to make application blazing fast by every flag possible
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "/Ox /GL /GF /Gy /fp:fast /Zc:inline /GR- /EHsc /MD /DNDEBUG")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG /OPT:REF /OPT:ICF")

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DUNICODE -D_UNICODE)

add_executable(wndmenu
    src/main.cpp
)

#set_target_properties(wndmenu PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

