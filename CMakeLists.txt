cmake_minimum_required(VERSION 3.21)
message("Using toolchain file ${CMAKE_TOOLCHAIN_FILE}.")

########################################################################################################################
## Define project
########################################################################################################################
project(
        MuFacialExpressionExtended
        VERSION 0.4.0
        DESCRIPTION "MuFacialExpressionExtended - VR by Acro."
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

        include/Utility.hpp
        include/nifUtil.h
        include/Config.h
        include/ConsoleLog.h
        include/PapyrusScripts.h
        include/Command.h
        include/MorphDataBase.h
        include/MorphManager.h
        include/ActorManager.h
        include/Store.h
        include/NiVector4.h
        include/Shader.h
        include/Hook.h

        include/Interface/Interface.h
        include/InterfaceImpl.h
)

set(sources
        src/nifUtil.cpp
        src/Config.cpp
        src/ConsoleLog.cpp
        src/PapyrusScripts.cpp
        src/Command.cpp
        src/MorphDataBase.cpp
        src/MorphManager.cpp
        src/ActorManager.cpp
        src/Shader.cpp
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
find_package(Directxtk CONFIG REQUIRED)

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
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        CommonLibSSE::CommonLibSSE
    PRIVATE
        magic_enum::magic_enum
)

target_precompile_headers(
    ${PROJECT_NAME}
        PRIVATE
        include/PCH.h)

target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/Zi>")
target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>")
target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>")

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
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Plugin${BUILD_NAME}VR/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Plugin${BUILD_NAME}VR/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Plugin${BUILD_NAME}VR/")

file(GLOB_RECURSE OUTPUT_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.dll")
file(GLOB_RECURSE OUTPUT_PDBS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.pdb")
file(GLOB_RECURSE OUTPUT_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.pex")
set_property(TARGET ${PROJECT_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_DLLS}")
set_property(TARGET ${PROJECT_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_PDBS}")
