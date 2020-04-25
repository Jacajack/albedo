cmake_minimum_required(VERSION 3.3)
project(albedo_demo)

# Source directory
set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}")

# Include directory
include_directories("${CMAKE_SOURCE_DIR}/../include" "${CMAKE_SOURCE_DIR}/../ext/include")

# General compilation flags
set(CXX_FLAGS_LIST
	"-Wall"
	"--std=c++17"
	"-pthread"
)
string(REPLACE ";" " " CXX_FLAGS_STR "${CXX_FLAGS_LIST}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS_STR}")

# Debug compilation flags
set(CXX_FLAGS_DEBUG_LIST
	"-g"
	"-fno-omit-frame-pointer"
	"-fsanitize=address"
	"-fsanitize=undefined"
	#"-fsanitize=thread"
	"-fno-builtin"
	"-DDEBUG"
	"-DALBEDO_INCLUDE_ENUM_MAP"
	"-DALBEDO_TRACE_EXCEPTIONS"
)
string(REPLACE ";" " " CXX_FLAGS_DEBUG_STR "${CXX_FLAGS_DEBUG_LIST}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_FLAGS_DEBUG_STR}")

# Release compilation flags
set( CXX_FLAGS_RELEASE_LIST
	"-s"
	"-O3"
	"-ffast-math"
	"-ftree-vectorize"
	"-flto"
	"-march=native"
	"-DNDEBUG"
)
string(REPLACE ";" " " CXX_FLAGS_RELEASE_STR "${CXX_FLAGS_RELEASE_LIST}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS_RELEASE_STR}")

link_directories("${PROJECT_SOURCE_DIR}/../build")

set_source_files_properties(
	"${PROJECT_SOURCE_DIR}/demo.cpp" PROPERTIES OBJECT_DEPENDS "${PROJECT_SOURCE_DIR}/../build/libalbedo.a"
)

# Libs
link_libraries(
	# Graphics
	"glfw"
	"GL"
	"GLEW"

	# Utils
	"dl"
	"assimp"

	# Boost
	"boost_system"
	"boost_filesystem"

	"albedo"
)

add_executable(
	demo
	"${PROJECT_SOURCE_DIR}/demo.cpp"
)