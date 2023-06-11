cmake_minimum_required(VERSION 3.25.1)
project(renamer VERSION 0.0.1)

configure_file(${CMAKE_SOURCE_DIR}/version.hpp.in 
               ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}/version.hpp)

add_executable(${PROJECT_NAME})

find_package(Boost 1.74.0 COMPONENTS filesystem system program_options log log_setup regex REQUIRED)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/options.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mask.cpp)