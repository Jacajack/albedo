cmake_minimum_required(VERSION 3.3)
project(albedo)

# Source directory
set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

# Include directory
include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/ext/include")

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
	#"-flto"
	"-march=native"
	"-DNDEBUG"
)
string(REPLACE ";" " " CXX_FLAGS_RELEASE_STR "${CXX_FLAGS_RELEASE_LIST}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS_RELEASE_STR}")

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
)

add_library(
	albedo
	"${PROJECT_SOURCE_DIR}/gl/debug.cpp"
	"${PROJECT_SOURCE_DIR}/gl/window.cpp"
	"${PROJECT_SOURCE_DIR}/gl/shader.cpp"
	"${PROJECT_SOURCE_DIR}/gl/program.cpp"
	"${PROJECT_SOURCE_DIR}/gl/buffer.cpp"
	"${PROJECT_SOURCE_DIR}/gl/vertex_array.cpp"
	"${PROJECT_SOURCE_DIR}/gl/uniform.cpp"
	"${PROJECT_SOURCE_DIR}/mesh.cpp"
	"${PROJECT_SOURCE_DIR}/simple_loaders.cpp"
	"${PROJECT_SOURCE_DIR}/fixed_vao.cpp"
	"${PROJECT_SOURCE_DIR}/camera.cpp"
	"${PROJECT_SOURCE_DIR}/albedo.cpp"
)
