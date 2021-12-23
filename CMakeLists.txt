cmake_minimum_required(VERSION 3.8)

project(Lilac LANGUAGES C CXX)
#	target_link_libraries(${PROJECT_NAME} "-framework CydiaSubstrate")
add_subdirectory("src")
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	add_subdirectory("test")
endif()
