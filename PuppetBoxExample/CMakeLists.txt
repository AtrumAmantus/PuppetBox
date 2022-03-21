cmake_minimum_required(VERSION 3.19)
project(PuppetBoxExample
        VERSION 0.0.1)

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(PBOX_SHARED_LIB ${OUTPUT_DIR}/PuppetBoxEngine.dll)
set(PBOX_STATIC_LIB ${OUTPUT_DIR}/PuppetBoxEngine.lib)
message("Add ${PBOX_SHARED_LIB}")
message("Add ${PBOX_STATIC_LIB}")
add_library(PuppetBoxEngineLib SHARED IMPORTED)
set_property(TARGET PuppetBoxEngineLib PROPERTY
        IMPORTED_LOCATION ${PBOX_SHARED_LIB})
set_property(TARGET PuppetBoxEngineLib PROPERTY
        IMPORTED_IMPLIB ${PBOX_STATIC_LIB})

set(LIBS PuppetBoxEngineLib)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${PUBLIC_HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(SDL_SHARED_DLIB ${DEP_DLIBRARY_DIR}/SDL2d.dll)
    message("Copy ${SDL_SHARED_DLIB} to ${OUTPUT_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME}
            COMMAND ${CMAKE_COMMAND} -E copy
            ${SDL_SHARED_DLIB}
            ${OUTPUT_DIR})
endif ()

set(SDL_SHARED_LIB ${DEP_DLIBRARY_DIR}/../Release/SDL2.dll)
message("Copy ${SDL_SHARED_LIB} to ${OUTPUT_DIR}")
add_custom_command(TARGET ${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy
        ${SDL_SHARED_LIB}
        ${OUTPUT_DIR})

set(SDL_NET_SHARED_LIB ${DEP_DLIBRARY_DIR}/SDL2_net.dll)
message("Copy ${SDL_NET_SHARED_LIB} to ${OUTPUT_DIR}")
add_custom_command(TARGET ${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy
        ${SDL_NET_SHARED_LIB}
        ${OUTPUT_DIR})