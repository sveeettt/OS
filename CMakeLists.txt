cmake_minimum_required(VERSION 3.10)
project(Lab1)

set(CMAKE_CXX_STANDARD 11)

# для винды
if(WIN32)
    add_compile_options(/utf-8) 
    add_definitions(-D_MBCS)
endif()

add_executable(Creator Creator.cpp Structure.h)
add_executable(Reporter Reporter.cpp Structure.h)
add_executable(Main Main.cpp Structure.h)

# после сборки Main
add_custom_command(TARGET Main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Creator>      # из Creator.exe
        $<TARGET_FILE_DIR:Main>     # в папку с Main.exe

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Reporter>
        $<TARGET_FILE_DIR:Main>
)