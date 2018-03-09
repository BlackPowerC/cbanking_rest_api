cmake_minimum_required(VERSION 3.5)

# gtest
include_directories(/usr/local/local/gtest)
link_directories(/usr/local/lib)

# pthread
include_directories(/usr/include)
link_directories(/lib/x86_64-linux-gnu/)

# rapisjson
include_directories(/usr/local/include/rapidjson)

set(CMAKE_CXX_FLAGS "-fpermissive")

add_executable(
        test_account_converter

        test_account_converter.cpp

        ../../../../src/Util/Converter/AccountConverter.cpp
        ../../../../src/Util/Converter/PersonConverter.cpp

        ../../../../src/Entity/Persons.cpp

        ../../../../src/Entity/Account.cpp

        ../../../../src/Entity/Operation.cpp
)

target_link_libraries(
        test_account_converter
        libgtest.a
        libgtest_main.a
        libpthread.so
)

target_compile_features(test_account_converter PUBLIC cxx_std_14)