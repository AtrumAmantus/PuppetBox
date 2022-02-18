cmake_minimum_required(VERSION 3.19)
project(PuppetBox
        VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

set(ARCH_TYPE x64 CACHE PATH "System architecture" FORCE)

add_definitions(-DPB_BUILD_VERSION="${CMAKE_PROJECT_VERSION}")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("Building in DEBUG mode")
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    message("Building in RELEASE mode")
endif ()

set(CMAKE_GENERATOR_PLATFORM ${ARCH_TYPE})

set(INCLUDE_DIR ../include)
file(GLOB_RECURSE PUBLIC_HEADER_FILES
        ${INCLUDE_DIR}/*.h)

set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin CACHE PATH "Build directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

set(DEP_DIRECTORY ${CMAKE_SOURCE_DIR}/dependencies)
set(DEP_DLIBRARY_DIR ${DEP_DIRECTORY}/shared/${ARCH_TYPE}/${CMAKE_BUILD_TYPE} CACHE PATH "Dependency Shared Libs" FORCE)

add_subdirectory(PuppetBoxEngine)
add_subdirectory(PuppetBoxExample)