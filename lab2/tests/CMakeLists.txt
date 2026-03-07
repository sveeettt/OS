# Тесты
add_executable(tests tests.cpp ../src/Threads.cpp)

target_include_directories(tests PRIVATE ../src)

# Регистрируем тест
add_test(NAME ThreadTests COMMAND tests)