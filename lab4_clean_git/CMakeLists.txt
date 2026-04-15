cmake_minimum_required(VERSION 3.15)
project(MessageQueue)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(message_queue
    src/main.cpp
    src/common/message_file.cpp
    src/receiver/receiver.cpp
    src/sender/sender.cpp
)

target_include_directories(message_queue PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/receiver
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sender
)

if(WIN32)
    target_link_libraries(message_queue PRIVATE ws2_32)
    set_target_properties(message_queue PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()

set_target_properties(message_queue PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Output will be in: ${CMAKE_BINARY_DIR}/bin")