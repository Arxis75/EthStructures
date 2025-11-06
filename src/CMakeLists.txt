file(GLOB BYTESET_PRIVATE_SRC
     CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

file(GLOB BYTESET_PUBLIC_HEADERS
     CONFIGURE_DEPENDS 
     "${PROJECT_SOURCE_DIR}/include/EthStructures/*.h"
     "${PROJECT_SOURCE_DIR}/include/EthStructures/*.tpp")

add_library(libEthStructures STATIC ${BYTESET_PRIVATE_SRC} ${BYTESET_PUBLIC_HEADERS} )

target_include_directories(libEthStructures
    PUBLIC  ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(libEthStructures PUBLIC givaro::givaro ethash::keccak libByteSet PRIVATE OpenSSL::Crypto ${CMAKE_DL_LIBS} nlohmann_json::nlohmann_json libcurl)