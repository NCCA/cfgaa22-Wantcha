cmake_minimum_required(VERSION 3.12)
#-------------------------------------------------------------------------------------------
# I'm going to use vcpk in most cases for our install of 3rd party libs
# this is going to check the environment variable for CMAKE_TOOLCHAIN_FILE and this must point to where
# vcpkg.cmake is in the University this is set in your .bash_profile to
# export CMAKE_TOOLCHAIN_FILE=/public/devel/2020/vcpkg/scripts/buildsystems/vcpkg.cmake
# to build see the NGL instructions 
# Windows :- mkdir build; cd build ; cmake -DCMAKE_PREFIX_PATH=~/NGL/ .. ; cmake --build . 
# Linux / Mac mkdir build; cd build; cmake -DCMAKE_PREFIX_PATH~/NGL/ .. ; make
#-------------------------------------------------------------------------------------------
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{CMAKE_TOOLCHAIN_FILE})
   set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})
endif()

# Name of the project
project(RendererBuild)
# This is the name of the Exe change this and it will change everywhere
set(TargetName Renderer)
# This will include the file NGLConfig.cmake, you need to add the location to this either using
# -DCMAKE_PREFIX_PATH=~/NGL or as a system environment variable. 
find_package(NGL CONFIG REQUIRED)
# Instruct CMake to run moc automatically when needed (Qt projects only)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/ui)
# find Qt libs
# find Qt libs first we check for Version 6
find_package(Qt6 COMPONENTS OpenGL Widgets QUIET )
if ( Qt6_FOUND )
    message("Found Qt6 Using that")
else()
    message("Found Qt5 Using that")
    find_package(Qt5 COMPONENTS OpenGL Widgets Core REQUIRED)
endif()

# use C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
# Set the name of the executable we want to build
add_executable(${TargetName})

# Add NGL include path
include_directories(include $ENV{HOME}/NGL/include ui)

target_sources(${TargetName} PRIVATE ${PROJECT_SOURCE_DIR}/src/main.cpp  
			${PROJECT_SOURCE_DIR}/src/NGLScene.cpp  
			${PROJECT_SOURCE_DIR}/src/NGLSceneMouseControls.cpp  
            ${PROJECT_SOURCE_DIR}/src/MainWindow.cpp 
            ${PROJECT_SOURCE_DIR}/src/Camera.cpp 
            ${PROJECT_SOURCE_DIR}/src/Lights.cpp 
            ${PROJECT_SOURCE_DIR}/src/PBRShaderManager.cpp 
            ${PROJECT_SOURCE_DIR}/src/Mesh.cpp 
            ${PROJECT_SOURCE_DIR}/src/ObjMesh.cpp 
            ${PROJECT_SOURCE_DIR}/src/SceneObject.cpp 
            ${PROJECT_SOURCE_DIR}/src/AssetManager.cpp 
            ${PROJECT_SOURCE_DIR}/src/MeshObject.cpp 
            ${PROJECT_SOURCE_DIR}/src/FrameBuffer.cpp 
            ${PROJECT_SOURCE_DIR}/src/Gizmos.cpp 
            ${PROJECT_SOURCE_DIR}/src/Transform.cpp 
            ${PROJECT_SOURCE_DIR}/src/TextureWidget.cpp 
			${PROJECT_SOURCE_DIR}/include/NGLScene.h 
            ${PROJECT_SOURCE_DIR}/include/MainWindow.h 
            ${PROJECT_SOURCE_DIR}/include/Camera.h 
            ${PROJECT_SOURCE_DIR}/include/Lights.h 
            ${PROJECT_SOURCE_DIR}/include/PBRShaderManager.h 
            ${PROJECT_SOURCE_DIR}/include/Mesh.h 
            ${PROJECT_SOURCE_DIR}/include/ObjMesh.h 
            ${PROJECT_SOURCE_DIR}/include/SceneObject.h  
            ${PROJECT_SOURCE_DIR}/include/Assets/AssetCache.h  
            ${PROJECT_SOURCE_DIR}/include/Assets/Asset.h  
            ${PROJECT_SOURCE_DIR}/include/Assets/AssetManager.h  
            ${PROJECT_SOURCE_DIR}/include/MeshObject.h  
            ${PROJECT_SOURCE_DIR}/include/FrameBuffer.h  
            ${PROJECT_SOURCE_DIR}/include/Gizmos.h  
            ${PROJECT_SOURCE_DIR}/include/Transform.h  
            ${PROJECT_SOURCE_DIR}/include/TextureWidget.h  
            ${PROJECT_SOURCE_DIR}/ui/MainWindow.ui
)

#file(GLOB_RECURSE SOURCES
#    "${FORMS_DIR}/*.ui"
#    "${FORMS_DIR}/*.qrc"
#    "${INCLUDE_DIR}/*.h"
#    "${SOURCE_DIR}/*.cpp"
#)

target_link_libraries(${TargetName} PRIVATE Qt::Widgets Qt::OpenGL NGL)

add_custom_target(${TargetName}CopyShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:${TargetName}>/shaders
)

add_custom_target(${TargetName}CopyMeshes ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/meshes
    $<TARGET_FILE_DIR:${TargetName}>/meshes
)

add_custom_target(${TargetName}CopyTextures ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/textures
    $<TARGET_FILE_DIR:${TargetName}>/textures
)
