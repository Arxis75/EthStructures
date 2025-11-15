file(GLOB ETHSTRUCTS_PRIVATE_SRC
     CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

file(GLOB ETHSTRUCTS_PUBLIC_HEADERS
     CONFIGURE_DEPENDS 
     "${PROJECT_SOURCE_DIR}/include/EthStructures/*.h"
     "${PROJECT_SOURCE_DIR}/include/EthStructures/*.tpp")

add_library(libEthStructures STATIC ${ETHSTRUCTS_PRIVATE_SRC} ${ETHSTRUCTS_PUBLIC_HEADERS} )

target_include_directories(libEthStructures
     PUBLIC
          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:include>
          #${PROJECT_SOURCE_DIR}/include
     PRIVATE
          ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(libEthStructures PUBLIC LibByteSet)