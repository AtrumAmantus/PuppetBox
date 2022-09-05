cmake_minimum_required(VERSION 3.23)
project(GPUDetails
        VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

set(ARCH_TYPE ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID})

message("Arch Type: ${ARCH_TYPE}")

set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin${ARCH_TYPE} CACHE PATH "Build directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

set(DEP_DIRECTORY ${CMAKE_SOURCE_DIR}/dependencies)
set(DEP_INCLUDES_DIR ${DEP_DIRECTORY}/include CACHE PATH "Dependency Includes" FORCE)
set(DEP_SLIBRARY_DIR ${DEP_DIRECTORY}/lib/${ARCH_TYPE}/${CMAKE_BUILD_TYPE} CACHE PATH "Dependency Static Libs" FORCE)
set(DEP_DLIBRARY_DIR ${DEP_DIRECTORY}/shared/${ARCH_TYPE}/${CMAKE_BUILD_TYPE} CACHE PATH "Dependency Shared Libs" FORCE)

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(SDL_FILENAME SDL2d)
else ()
    set(SDL_FILENAME SDL2)
endif ()

set(SDL_SHARED_LIB ${DEP_DLIBRARY_DIR}/${SDL_FILENAME}.dll)
set(SDL_STATIC_LIB ${DEP_SLIBRARY_DIR}/${SDL_FILENAME}.lib)
message("Add ${SDL_SHARED_LIB}")
message("Add ${SDL_STATIC_LIB}")
add_library(SDL2Dep SHARED IMPORTED)
set_property(TARGET SDL2Dep PROPERTY
        IMPORTED_LOCATION ${SDL_SHARED_LIB})
set_property(TARGET SDL2Dep PROPERTY
        IMPORTED_IMPLIB ${SDL_STATIC_LIB})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(SDL_MAIN_FILENAME SDL2maind)
else ()
    set(SDL_MAIN_FILENAME SDL2main)
endif ()

set(SDL_MAIN_STATIC_LIB ${DEP_SLIBRARY_DIR}/${SDL_MAIN_FILENAME}.lib)
message("Add ${SDL_MAIN_STATIC_LIB}")
add_library(SDL2MainDep STATIC IMPORTED)
set_property(TARGET SDL2MainDep PROPERTY
        IMPORTED_LOCATION ${SDL_MAIN_STATIC_LIB})

find_package(OpenGL REQUIRED)

set(LIBS opengl32 SDL2Dep SDL2MainDep)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE ${DEP_INCLUDES_DIR})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(SDL_SHARED_DLIB ${DEP_DLIBRARY_DIR}/SDL2d.dll)
    message("Copy ${SDL_SHARED_DLIB} to ${OUTPUT_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME}
            COMMAND ${CMAKE_COMMAND} -E copy
            ${SDL_SHARED_DLIB}
            ${OUTPUT_DIR})
else()
    set(SDL_SHARED_LIB ${DEP_DLIBRARY_DIR}/SDL2.dll)
    message("Copy ${SDL_SHARED_LIB} to ${OUTPUT_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME}
            COMMAND ${CMAKE_COMMAND} -E copy
            ${SDL_SHARED_LIB}
            ${OUTPUT_DIR})
endif ()
