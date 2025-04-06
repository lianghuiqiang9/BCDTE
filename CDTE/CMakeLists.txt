cmake_minimum_required(VERSION 3.16)

project(cdte)
find_package(SEAL)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(cdte SHARED ${SOURCE_FILES})

target_include_directories(cdte PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(cdte SEAL::seal)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
install(TARGETS cdte LIBRARY DESTINATION lib)
# install(FILES ${PROJECT_SOURCE_DIR}/include/cmp.h ${PROJECT_SOURCE_DIR}/include/node.h ${PROJECT_SOURCE_DIR}/include/pdte.h ${PROJECT_SOURCE_DIR}/include/utils.h ${PROJECT_SOURCE_DIR}/include/json.hpp   DESTINATION ../include)  
