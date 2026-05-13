cmake_minimum_required(VERSION 3.10)
project(FileAccessControl)

# Set C++ standard to C++98
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ��� Visual Studio - ���������� �������������� � ������������ ��������
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    # ���������� ��������� �������������� MSVC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996 /wd4267 /wd4244")
endif()

# Find required packages
find_package(Threads REQUIRED)

# Build tests option
option(BUILD_TESTS "Build tests" OFF)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Set source directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_SOURCE_DIR}/tests)

# Common library
add_library(common_lib 
    ${SRC_DIR}/common.cpp
    ${SRC_DIR}/protocol.cpp
    ${SRC_DIR}/file_manager.cpp
    ${SRC_DIR}/access_manager.cpp
)

# Server executable
add_executable(server 
    ${SRC_DIR}/server.cpp
    ${SRC_DIR}/named_pipe.cpp
    ${SRC_DIR}/access_manager.cpp
)
target_link_libraries(server common_lib)

# Client executable
add_executable(client 
    ${SRC_DIR}/client.cpp
    ${SRC_DIR}/named_pipe.cpp
)
target_link_libraries(client common_lib)

# ��� Windows ��������� ����������� ����������
if(WIN32)
    target_link_libraries(server ws2_32)
    target_link_libraries(client ws2_32)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if(BUILD_TESTS)
    enable_testing()

    add_executable(tests 
        ${TEST_DIR}/test_common.cpp
        ${TEST_DIR}/test_protocol.cpp
        ${TEST_DIR}/test_file_manager.cpp
        ${TEST_DIR}/test_access_manager.cpp
    )

    target_link_libraries(tests 
        common_lib 
        GTest::gtest 
        GTest::gtest_main 
        ${CMAKE_THREAD_LIBS_INIT}
    )

    add_test(NAME unit_tests COMMAND tests)
endif()

# Set executable output directory for easier deployment
set_target_properties(server client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# ��� Visual Studio - ��������� ����� � Solution Explorer
if(MSVC)
    # ����������� ������ � �����
    source_group("Header Files\\include" FILES 
        ${CMAKE_SOURCE_DIR}/include/access_manager.h
        ${CMAKE_SOURCE_DIR}/include/common.h
        ${CMAKE_SOURCE_DIR}/include/file_manager.h
        ${CMAKE_SOURCE_DIR}/include/named_pipe.h
        ${CMAKE_SOURCE_DIR}/include/protocol.h
    )
    
    source_group("Source Files\\src" FILES 
        ${SRC_DIR}/access_manager.cpp
        ${SRC_DIR}/client.cpp
        ${SRC_DIR}/common.cpp
        ${SRC_DIR}/file_manager.cpp
        ${SRC_DIR}/named_pipe.cpp
        ${SRC_DIR}/protocol.cpp
        ${SRC_DIR}/server.cpp
    )
    
    # ��������� ���������� �������
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT server)
    
    # ��������� ������������ Debug/Release
    set_target_properties(server PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    set_target_properties(client PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# �������������� ��������� ��� �������
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
    else()
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    endif()
endif()

# ���������� � ������
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
if(WIN32)
    message(STATUS "Windows build detected")
endif()

# ����������� ����������� ������ � �������� ����� ��� ��������
if(WIN32)
    add_custom_command(TARGET server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:server>"
        "${CMAKE_SOURCE_DIR}/server.exe"
    )
    add_custom_command(TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:client>"
        "${CMAKE_SOURCE_DIR}/client.exe"
    )
endif()