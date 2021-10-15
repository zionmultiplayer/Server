cmake_minimum_required(VERSION 3.7)
project(Zion)

option(BUILD_TESTS "Build tests" OFF)

add_subdirectory(${PROJECT_SOURCE_DIR}/Vendor/RakNet)
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/Source/*.cc)
add_library(zion STATIC ${SOURCES})
target_link_libraries(zion PUBLIC raknet)
target_include_directories(zion PUBLIC ${PROJECT_SOURCE_DIR}/Include)

if(BUILD_TESTS)
    add_executable(bare ${PROJECT_SOURCE_DIR}/Tests/Bare/Bare.cc)
    target_link_libraries(bare PRIVATE zion)

    add_executable(chatbubble ${PROJECT_SOURCE_DIR}/Tests/ChatBubble/ChatBubble.cc)
    target_link_libraries(chatbubble PRIVATE zion)

    add_executable(progressbar ${PROJECT_SOURCE_DIR}/Tests/ProgressBar/ProgressBar.cc)
    target_link_libraries(progressbar PRIVATE zion)
endif()