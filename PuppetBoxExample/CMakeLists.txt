cmake_minimum_required(VERSION 3.19)
project(PuppetBoxExample
        VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(PuppetBoxEngineLib SHARED IMPORTED)
set_property(TARGET PuppetBoxEngineLib PROPERTY
        IMPORTED_LOCATION ${OUTPUT_DIR}/PuppetBoxEngine.dll)
set_property(TARGET PuppetBoxEngineLib PROPERTY
        IMPORTED_IMPLIB ${OUTPUT_DIR}/PuppetBoxEngine.lib)

set(LIBS PuppetBoxEngineLib)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${PUBLIC_HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

add_custom_command(TARGET ${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy
        ${DEP_DLIBRARY_DIR}/SDL2.dll
        ${OUTPUT_DIR})