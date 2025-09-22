#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <wrl/client.h>

#include <filesystem>

// Defined relative to the project directory
#define SHADER_PATH "../Temp/Shaders/"

#define LOAD_VERTEX_SHADER(name, vector) LoadBinaryFile(SHADER_PATH name "_vs.dxil", vector)
#define LOAD_PIXEL_SHADER(name, vector) LoadBinaryFile(SHADER_PATH name "_ps.dxil", vector)
#define LOAD_COMPUTE_SHADER(name, vector) LoadBinaryFile(SHADER_PATH name "_cs.dxil", vector)

// Helper to load a binary file into a std::vector<uint8>
inline void LoadBinaryFile(const String& path, Vector<uint8>& outFile)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Failed to open shader file: " + path);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    outFile.resize(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(outFile.data()), size))
        throw std::runtime_error("Failed to read shader file: " + path);
}