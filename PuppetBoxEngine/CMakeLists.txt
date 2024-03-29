cmake_minimum_required(VERSION 3.19)
project(PuppetBoxEngine
        VERSION 0.0.1)

set(DEP_INCLUDES_DIR ${DEP_DIRECTORY}/include CACHE PATH "Dependency Includes" FORCE)
set(DEP_SLIBRARY_DIR ${DEP_DIRECTORY}/lib/${ARCH_TYPE}/${CMAKE_BUILD_TYPE} CACHE PATH "Dependency Static Libs" FORCE)

# Generate asset data
function(create_resources dir output)
    # Create / overwrite file
    file(WRITE ${output} "")
    # Read each file in target dir
    file(GLOB bins ${dir}/*)
    foreach(bin ${bins})
        # Create macro variable
        string(REGEX MATCH "([^/]+)$" filename ${bin})
        string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
        string(TOUPPER ${filename} macro_variable)
        # Parse file data
        file(READ ${bin} filedata)
        string(REGEX REPLACE "(\r\n)|\n" "\\\\n" filedata "${filedata}")
        # Add macro definition
        file(APPEND ${output} "#define DEFAULT_ASSET_${macro_variable} \"${filedata}\"\n")
    endforeach()
endfunction()

# Parse all default asset data to header
create_resources("assets/" "generated/DefaultAssets.h")

file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

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

set(SDL_NET_SHARED_LIB ${DEP_DLIBRARY_DIR}/SDL2_net.dll)
set(SDL_NET_STATIC_LIB ${DEP_SLIBRARY_DIR}/SDL2_net.lib)
message("Add ${SDL_NET_SHARED_LIB}")
message("Add ${SDL_NET_STATIC_LIB}")
add_library(SDL2NetDep SHARED IMPORTED)
set_property(TARGET SDL2NetDep PROPERTY
        IMPORTED_LOCATION ${SDL_NET_SHARED_LIB})
set_property(TARGET SDL2NetDep PROPERTY
        IMPORTED_IMPLIB ${SDL_NET_STATIC_LIB})

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

set(ZIP_STATIC_LIB ${DEP_SLIBRARY_DIR}/zip.lib)
message("Add ${ZIP_STATIC_LIB}")
add_library(ZipDep STATIC IMPORTED)
set_property(TARGET ZipDep PROPERTY
        IMPORTED_LOCATION ${ZIP_STATIC_LIB})

add_compile_definitions(PUPPETBOXENGINE_EXPORTS)

find_package(OpenGL REQUIRED)

set(LIBS opengl32 SDL2Dep SDL2NetDep SDL2MainDep freeType ZipDep)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES} ${PUBLIC_HEADER_FILES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR} ${DEP_INCLUDES_DIR})