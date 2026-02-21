cmake_minimum_required(VERSION 3.21)
message("Using toolchain file ${CMAKE_TOOLCHAIN_FILE}.")

########################################################################################################################
## Define project
########################################################################################################################
project(
        MuFacialExpressionExtended
        VERSION 0.5.1
        DESCRIPTION "MuFacialExpressionExtended - SE/AE by Acro."
        LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

include(GNUInstallDirs)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY
)

set(headers
        ExtraRE/B/BSFaceGenBaseMorphExtraData.h

        include/PrismaUI/PrismaUI_API.h
        include/PrismaUI/PrismaUI.h
        include/Utility.hpp
        include/nifUtil.h
        include/Config.h
        include/lString.h
        include/ConsoleLog.h
        include/InputManager.h
        include/PapyrusScripts.h
        include/Command.h
        include/MorphDataBase.h
        include/MorphManager.h
        include/ActorManager.h
        include/Store.h
        include/Hook.h

        include/Interface/Interface.h
        include/Interface/InterfaceImpl.h
        include/Interface/OldInterface.h
        include/Interface/OldInterfaceImpl.h
)

set(sources
        include/PrismaUI/PrismaUI.cpp
        src/nifUtil.cpp
        src/Config.cpp
        src/ConsoleLog.cpp
        src/InputManager.cpp
        src/PapyrusScripts.cpp
        src/Command.cpp
        src/MorphDataBase.cpp
        src/MorphManager.cpp
        src/ActorManager.cpp
        src/Hook.cpp
        src/Store.cpp
        src/Main.cpp

        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
)

source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        ${headers}
        ${sources})

########################################################################################################################
## Configure target DLL
########################################################################################################################
find_package(CommonLibSSE REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(JsonCpp CONFIG REQUIRED)
find_package(Directxtk CONFIG REQUIRED)
find_package(xxhash CONFIG REQUIRED)
find_package(detours)

add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${headers} ${sources})
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

target_compile_features(
    ${PROJECT_NAME}
    PUBLIC
        cxx_std_23
)

target_compile_definitions(
    ${PROJECT_NAME}
    PUBLIC
        _UNICODE
        UNICODE
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
        $<INSTALL_INTERFACE:src>
        ${ARTICUNO_INCLUDE_DIRS}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ExtraRE>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extern/rapidhash/rapidhash-c/cpp>
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        CommonLibSSE::CommonLibSSE
    PRIVATE
        magic_enum::magic_enum
        JsonCpp::JsonCpp
        xxHash::xxhash
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/x64-windows-static-md/lib/detours.lib>
)

target_precompile_headers(
    ${PROJECT_NAME}
        PRIVATE
        include/PCH.h)
        
target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/sdl;/utf-8;/Zi;/permissive-;/Zc:preprocessor;/Zc:inline;/JMC-;/O2;/Ob3>")
target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>")

install(TARGETS ${PROJECT_NAME}
        DESTINATION "${CMAKE_INSTALL_LIBDIR}")

########################################################################################################################
## Shader Script
########################################################################################################################
#set(shader_sources 
#        ${CMAKE_CURRENT_SOURCE_DIR}/Shader/Morph.hlsl
#)
#set(shader_header "${CMAKE_CURRENT_SOURCE_DIR}/Shader/Shader.h")

#add_custom_target(ValidateShaders ALL 
#    COMMENT "Running Shader Validation"
#)
#foreach(shader ${shader_sources})
#    get_filename_component(shader_name ${shader} NAME_WE)
#    set(compiledShader "${CMAKE_CURRENT_SOURCE_DIR}/Shader/Compiled/${shader_name}.cso")
#
#    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
#        TARGET ValidateShaders
#        COMMAND fxc.exe /T cs_5_0 /E main /Fo ${compiledShader} /nologo ${shader}
#        COMMAND ${CMAKE_COMMAND} -E echo "Shader valid : ${shader}"
#    )
#endforeach()

#find_package(Python3 REQUIRED)
#add_custom_target(GenerateShaderHeader ALL
#    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/Shader/Compiled/"
#    COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/Shader/GenerateShaderHeader.py" ${shader_header} ${shader_sources}
#    DEPENDS ${shader_sources}
#    COMMENT "Generating Shader Header"
#)
#add_dependencies(GenerateShaderHeader ValidateShaders)
#add_dependencies(${PROJECT_NAME} GenerateShaderHeader)

########################################################################################################################
## Automatic plugin deployment
########################################################################################################################
# Automatic deployment to FOMOD directory.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_NAME "Debug")
else()
    set(BUILD_NAME "Release")
endif()
install(DIRECTORY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Papyrus${BUILD_NAME}/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Plugin${BUILD_NAME}/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Plugin${BUILD_NAME}/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Plugin${BUILD_NAME}/")

file(GLOB_RECURSE OUTPUT_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.dll")
file(GLOB_RECURSE OUTPUT_PDBS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.pdb")
file(GLOB_RECURSE OUTPUT_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.pex")
set_property(TARGET ${PROJECT_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_DLLS}")
set_property(TARGET ${PROJECT_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_PDBS}")
