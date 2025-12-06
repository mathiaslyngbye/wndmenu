cmake_minimum_required(VERSION 3.15)
project(wndmenu)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

set(CMAKE_CXX_FLAGS_RELEASE "/Ox /GL /GF /Gy /fp:fast /Zc:inline /GR- /EHsc /DNDEBUG")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG /OPT:REF /OPT:ICF")

add_compile_definitions(UNICODE _UNICODE)

add_executable(wndmenu
    src/main.cpp
)

# Optional GUI-only build
set_target_properties(wndmenu PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")