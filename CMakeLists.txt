cmake_minimum_required( VERSION 3.3 )
project( albedo )

# Source directory
set( PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src" )

# Include directory
include_directories( "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/ext/include" )

# General compilation flags
set( CXX_FLAGS_LIST
	"-Wall"
	"--std=c++17"
	"-pthread"
)
string( REPLACE ";" " " CXX_FLAGS_STR "${CXX_FLAGS_LIST}" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS_STR}" )

# Debug compilation flags
set( CXX_FLAGS_DEBUG_LIST
	"-g"
	"-fno-omit-frame-pointer"
	"-fsanitize=address"
	"-fsanitize=undefined"
	#"-fsanitize=thread"
	"-fno-builtin"
	"-DALBEDO_INCLUDE_ENUM_MAP"
	"-DALBEDO_TRACE_EXCEPTIONS"
)
string( REPLACE ";" " " CXX_FLAGS_DEBUG_STR "${CXX_FLAGS_DEBUG_LIST}" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_FLAGS_DEBUG_STR}" )

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
string( REPLACE ";" " " CXX_FLAGS_RELEASE_STR "${CXX_FLAGS_RELEASE_LIST}" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS_RELEASE_STR}" )

# Libs
link_libraries(
	# Graphics
	"glfw"
	"GL"
	"GLEW"

	# Utils
	"dl"
	"assimp"
	"yaml-cpp"

	# Boost
	"boost_system"
	"boost_filesystem"
)

add_executable(
	albedo
	"${PROJECT_SOURCE_DIR}/albedo.cpp"
	"${PROJECT_SOURCE_DIR}/log.cpp"
	"${PROJECT_SOURCE_DIR}/exception.cpp"
	"${PROJECT_SOURCE_DIR}/shared_library.cpp"
	"${PROJECT_SOURCE_DIR}/game.cpp"
	"${PROJECT_SOURCE_DIR}/utils.cpp"
	"${PROJECT_SOURCE_DIR}/resource_engine/resource.cpp"
	"${PROJECT_SOURCE_DIR}/resource_engine/resource_engine.cpp"
	"${PROJECT_SOURCE_DIR}/gfx/window.cpp"
	"${PROJECT_SOURCE_DIR}/gfx/gl/dispatcher.cpp"
	"${PROJECT_SOURCE_DIR}/gfx/gl/error.cpp"
	"${PROJECT_SOURCE_DIR}/gfx/gl/shader.cpp"
	"${PROJECT_SOURCE_DIR}/gfx/gl/program.cpp"
	"${PROJECT_SOURCE_DIR}/gfx/gl/uniform.cpp"
	"${PROJECT_SOURCE_DIR}/gfx/gl/debug.cpp"
	"${PROJECT_SOURCE_DIR}/gfx/gl/vao.cpp"
	"${PROJECT_SOURCE_DIR}/gfx/mesh.cpp"
	"${PROJECT_SOURCE_DIR}/gfx/camera.cpp"
	"${PROJECT_SOURCE_DIR}/gfx/model.cpp"
	"${PROJECT_SOURCE_DIR}/loaders/yaml_model_loader.cpp"
	"${PROJECT_SOURCE_DIR}/loaders/assimp_mesh_loader.cpp"
	"${PROJECT_SOURCE_DIR}/loaders/glsl_loader.cpp"
	"${PROJECT_SOURCE_DIR}/loaders/generic_yaml_resource_loader.cpp"
)
