cmake_minimum_required(VERSION 3.22)
project(freetype_conversion
        VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

set(ARCH_TYPE ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID})

message("Building in ${CMAKE_BUILD_TYPE} mode")
message("Target architecture: ${ARCH_TYPE}")

set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin${ARCH_TYPE} CACHE PATH "Build directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

set(DEP_DIRECTORY ${CMAKE_SOURCE_DIR}/dependencies)
set(DEP_INCLUDES_DIR ${DEP_DIRECTORY}/include CACHE PATH "Dependency Includes" FORCE)
set(DEP_SLIBRARY_DIR ${DEP_DIRECTORY}/lib/${ARCH_TYPE}/${CMAKE_BUILD_TYPE} CACHE PATH "Dependency Static Libs" FORCE)

message("Dependencies in ${DEP_INCLUDES_DIR} and ${DEP_SLIBRARY_DIR}")

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(FREETYPE_FILENAME freetyped)
else ()
    set(FREETYPE_FILENAME freetype)
endif ()

set(FREE_TYPE_STATIC_LIB ${DEP_SLIBRARY_DIR}/${FREETYPE_FILENAME}.lib)
message("Add ${FREE_TYPE_STATIC_LIB}")
add_library(freeType STATIC IMPORTED)
set_property(TARGET freeType PROPERTY
        IMPORTED_LOCATION ${FREE_TYPE_STATIC_LIB})

set(LIBS freeType)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE ${DEP_INCLUDES_DIR})
