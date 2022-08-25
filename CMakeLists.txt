project(ThorRender)
cmake_minimum_required(VERSION 3.21)

# Source root
set(_src_root_path "${CMAKE_CURRENT_SOURCE_DIR}/source")

# Accumulate source files
file(GLOB_RECURSE _source_files 
    LIST_DIRECTORIES false
    "${_src_root_path}/*.c*"
    "${_src_root_path}/*.h*"
)

# Create project
add_executable(ThorRender ${_source_files})

# Create project filters
foreach(_source IN ITEMS ${_source_files})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

# Includes
target_include_directories(ThorRender PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

#DX12 Libraries
target_link_libraries(ThorRender PRIVATE
    d3d12.lib
    dxgi.lib
    dxguid.lib
)