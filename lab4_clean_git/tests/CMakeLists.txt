# Подключаем GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Тест для message_file
add_executable(message_file_test
    message_file_test.cpp
)

target_include_directories(message_file_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/common
)

target_link_libraries(message_file_test
    PRIVATE message_file
    PRIVATE gtest
    PRIVATE gtest_main
)

add_test(NAME MessageFileTest COMMAND message_file_test)

# Тест для receiver
add_executable(receiver_test
    receiver_test.cpp
)

target_include_directories(receiver_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/common
    ${CMAKE_SOURCE_DIR}/src/receiver
)

target_link_libraries(receiver_test
    PRIVATE message_file
    PRIVATE gtest
    PRIVATE gtest_main
)

add_test(NAME ReceiverTest COMMAND receiver_test)

# Тест для sender
add_executable(sender_test
    sender_test.cpp
)

target_include_directories(sender_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/common
    ${CMAKE_SOURCE_DIR}/src/sender
)

target_link_libraries(sender_test
    PRIVATE message_file
    PRIVATE gtest
    PRIVATE gtest_main
)

add_test(NAME SenderTest COMMAND sender_test)