cmake_minimum_required(VERSION 3.7)
project(RakNet)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/Source/*.cpp)
add_library(raknet STATIC ${SOURCES})
target_include_directories(raknet
    PRIVATE ${PROJECT_SOURCE_DIR}/Include/RakNet/
    INTERFACE ${PROJECT_SOURCE_DIR}/Include/)

if (WIN32)
    target_link_libraries(raknet PUBLIC ws2_32)
elseif(UNIX)
    target_link_libraries(raknet PUBLIC pthread)
endif()
