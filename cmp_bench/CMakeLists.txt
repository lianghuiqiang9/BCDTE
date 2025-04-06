cmake_minimum_required(VERSION 3.16)

project(cmp)

find_package(SEAL)

link_directories (${PROJECT_SOURCE_DIR}/../CDTE/lib)

add_executable(tecmp tecmp.cpp)
target_include_directories(tecmp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../CDTE/include)
target_link_libraries (tecmp SEAL::seal libcdte.so)

add_executable(tecmp_norm tecmp_norm.cpp)
target_include_directories(tecmp_norm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../CDTE/include)
target_link_libraries(tecmp_norm SEAL::seal libcdte.so)

add_executable(rdcmp rdcmp.cpp)
target_include_directories(rdcmp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../CDTE/include)
target_link_libraries(rdcmp SEAL::seal libcdte.so)

add_executable(cdcmp cdcmp.cpp)
target_include_directories(cdcmp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../CDTE/include)
target_link_libraries(cdcmp SEAL::seal libcdte.so)

add_executable(time_test time_test.cpp)
target_link_libraries(time_test SEAL::seal)

add_executable(depth_test depth_test.cpp)
target_link_libraries(depth_test SEAL::seal)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
