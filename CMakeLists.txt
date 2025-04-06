cmake_minimum_required(VERSION 3.16)

project(cmp)

find_package(SEAL)

link_directories (${PROJECT_SOURCE_DIR}/CDTE/lib)

add_executable(pir_cdcmp_cdte pir_cdcmp_cdte.cpp)
target_include_directories(pir_cdcmp_cdte PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CDTE/include)
target_link_libraries (pir_cdcmp_cdte SEAL::seal libcdte.so)

add_executable(new_tree_and_data new_tree_and_data.cpp)
target_include_directories(new_tree_and_data PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CDTE/include)
target_link_libraries(new_tree_and_data SEAL::seal libcdte.so)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")