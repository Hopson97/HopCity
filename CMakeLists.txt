cmake_minimum_required(VERSION 3.10)

#Set up project
project(HopCity VERSION 1.0)

IF (WIN32)
    # set stuff for windows
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    set(SFML_STATIC_LIBRARIES TRUE)
    set(SFML_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/SFML-2.5.1/lib/cmake/SFML")
    file(COPY libs/SFML-2.5.1/bin/openal32.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY "Data"  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
ELSE ()
    # set stuff for other systems
    include(${CMAKE_BINARY_DIR}/../conanbuildinfo.cmake)
    conan_basic_setup()
ENDIF ()

#Set executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/Animation.cpp
    src/Screen.cpp 
    src/Profiler.cpp
    
    src/Game/Construction.cpp
    src/Game/TileMap.cpp
    src/Game/ScreenGame.cpp
    src/Game/ScreenMainMenu.cpp
    src/Game/Camera.cpp
    src/Game/StructureMap.cpp
    src/Game/Resources.cpp
    src/Game/Renderer.cpp
    src/Game/WorldGeneration.cpp

    src/Game/Registry/TileRegistry.cpp
    src/Game/Registry/StructureRegistry.cpp
    )

#Set C++17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

#Set flags
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -pedantic)
endif ()

#Set module path

#Find libraries
find_package(Threads)


IF (WIN32)
    find_package(SFML 2.5.1 COMPONENTS system window graphics network audio REQUIRED)
    find_package(OpenGL REQUIRED)
    add_subdirectory(deps/imgui_sfml)
    add_subdirectory(deps/glm)
    target_include_directories(${PROJECT_NAME} PRIVATE deps)
    target_link_libraries(${PROJECT_NAME} sfml-audio sfml-graphics sfml-window sfml-system sfml-network ${OPENGL_LIBRARIES} imgui_sfml )

ELSE ()
    add_subdirectory(deps/imgui_sfml)
    target_include_directories(${PROJECT_NAME} PRIVATE deps)
    target_link_libraries(${PROJECT_NAME}
            Threads::Threads
            ${CONAN_LIBS}
            imgui_sfml
            )
ENDIF ()
#Finally


