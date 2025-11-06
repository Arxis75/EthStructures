file(GLOB SOURCES "*.h" "*.cpp")

target_sources(EthStructuresTest PUBLIC ${SOURCES})

add_subdirectory(constructors)