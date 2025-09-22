cmake_minimum_required(VERSION 4.1.1)
project(ThorRender)

# Enable folder organization in the Solution Explorer
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(submodules/DirectX-Headers)
set_target_properties(DirectX-Headers PROPERTIES FOLDER "external/DirectX")
set_target_properties(DirectX-Guids PROPERTIES FOLDER "external/DirectX")

# Source root
set(_src_root_path "${CMAKE_CURRENT_SOURCE_DIR}/source")

# Accumulate source files
file(GLOB_RECURSE _source_files 
    LIST_DIRECTORIES false
    "${_src_root_path}/*.c*"
    "${_src_root_path}/*.h*"
    "${_src_root_path}/*.vshader"
    "${_src_root_path}/*.pshader"
    "${_src_root_path}/*.cshader"
)

# Accumulate shader files
file(GLOB_RECURSE _shader_files 
    LIST_DIRECTORIES false
    "${_src_root_path}/*.vshader"
    "${_src_root_path}/*.pshader"
    "${_src_root_path}/*.cshader"
)

# Set property to skip compilation for all shader files
set_source_files_properties(${_shader_files} PROPERTIES HEADER_FILE_ONLY TRUE)

# Create project
add_executable(ThorRender ${_source_files})

set_target_properties(ThorRender PROPERTIES
       WIN32_EXECUTABLE TRUE
   )

# Create project filters
foreach(_source IN ITEMS ${_source_files})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

# Add dependencies
add_dependencies(ThorRender DirectX-Headers)

# Includes
target_include_directories(ThorRender PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/submodules/DirectX-Headers/include
)

#DX12 Libraries
target_link_libraries(ThorRender PRIVATE
    d3d12.lib
    dxgi.lib
    dxguid.lib
)