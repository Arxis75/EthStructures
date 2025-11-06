cmake_minimum_required(VERSION 3.16)

project(EthStructuresTest VERSION 1.0 LANGUAGES CXX)

set(PROJECT_SOURCES main.cpp)

add_executable(EthStructuresTest ${PROJECT_SOURCES})

#Defines the root folder(s) for all targets
target_include_directories(
    EthStructuresTest
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${PROJECT_SOURCE_DIR}/src      #PRIVATE ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(EthStructuresTest PRIVATE libEthStructures GTest::gtest)

add_subdirectory(unit_tests)

enable_testing()

include(GoogleTest)
gtest_discover_tests(EthStructuresTest)