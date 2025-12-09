set(PROJECT_SOURCES main.cpp)

add_executable(EthStructuresApp ${PROJECT_SOURCES})

#Defines the root folder(s) for all targets
target_include_directories(
    EthStructuresApp
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${PROJECT_SOURCE_DIR}/src      #PRIVATE ${PROJECT_SOURCE_DIR}/src
)

#target_link_libraries(EthStructuresApp PUBLIC libEthStructures)
target_link_libraries(EthStructuresApp PUBLIC libEthStructures PRIVATE nlohmann_json::nlohmann_json libcurl evmone evmc)    #for testing purpose