cmake_minimum_required(VERSION 3.10)

#Set up project
project(HopCity  VERSION 1.0)

include(${CMAKE_BINARY_DIR}/../conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB_RECURSE SOURCES "src/*.cpp") #glob all the source files intp one place
#Set executable
add_executable(${PROJECT_NAME} ${SOURCES})

#Set C++17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

#Set flags
if(MSVC)
  	target_compile_options(${PROJECT_NAME} PRIVATE 
    	/W4 /WX)
else()
  	target_compile_options(${PROJECT_NAME} PRIVATE 
		-Wall -Wextra -pedantic)
endif()

#Set module path

#Find libraries
find_package(Threads)

add_subdirectory(deps/imgui_sfml)
target_include_directories(${PROJECT_NAME} PRIVATE deps)



#Finally
target_link_libraries(${PROJECT_NAME} 
    Threads::Threads 
    ${CONAN_LIBS}

	imgui_sfml
)

