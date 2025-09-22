#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <wrl/client.h>

#include <filesystem>

// Defined relative to the project directory
#define SHADER_PATH "../Temp/Shaders/"

#define LOAD_VERTEX_SHADER(name) LoadBinaryFile(SHADER_PATH name "_vs.dxil")
#define LOAD_PIXEL_SHADER(name) LoadBinaryFile(SHADER_PATH name "_ps.dxil")
#define LOAD_COMPUTE_SHADER(name) LoadBinaryFile(SHADER_PATH name "_cs.dxil")

// Helper to load a binary file into a std::vector<uint8_t>
inline std::vector<uint8_t> LoadBinaryFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Failed to open shader file: " + path);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        throw std::runtime_error("Failed to read shader file: " + path);
    return buffer;
}