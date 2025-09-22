#pragma once
#include <cstdint>
#include <cmath>
#include <numbers>
#include <algorithm>
#include <numeric>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <stdexcept>

#include <wrl/client.h>

#include "DirectXMath.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

// STL types
template <class _Ty, class _Alloc = std::allocator<_Ty>>
using Vector = std::vector<_Ty, _Alloc>;

template <class _Kty, class _Ty, class _Hasher = std::hash<_Kty>, class _Keyeq = std::equal_to<_Kty>, class _Alloc = std::allocator<std::pair<const _Kty, _Ty>>>
using HashMap = std::unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>;

using String = std::string;

// Pointer types
template<class T>
using SharedPtr = std::shared_ptr<T>;

template<class T, class... Args>
constexpr SharedPtr<T> MakeShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<class T>
using UniquePtr = std::unique_ptr<T>;

template<class T, class... Args>
constexpr UniquePtr<T> MakeUnique(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<class T>
using WeakPtr = std::weak_ptr<T>;

// Scalar types
using int64 = int64_t;
using int32 = int32_t;
using int16 = int16_t;
using int8 = int8_t;
using uint64 = uint64_t;
using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8 = uint8_t;
using uint = unsigned int;

using float32 = float;
using float64 = double;

// Vector types (DirectXMath)
using float2 = XMFLOAT2;
using float3 = XMFLOAT3;
using float4 = XMFLOAT4;

using int2 = XMINT2;
using int3 = XMINT3;
using int4 = XMINT4;

using uint2 = XMUINT2;
using uint3 = XMUINT3;
using uint4 = XMUINT4;

using bool2 = XMUINT2;
using bool3 = XMUINT3;
using bool4 = XMUINT4;


// Matrix types (DirectXMath)
using float3x3 = XMFLOAT3X3;
using float4x4 = XMFLOAT4X4;
using float4x3 = XMFLOAT4X3;
using float3x4 = XMFLOAT3X4;

// Constants
constexpr float PI = XM_PI;
constexpr float TWO_PI = XM_2PI;
constexpr float HALF_PI = XM_PIDIV2;
constexpr float QUARTER_PI = XM_PIDIV4;
constexpr float DEG_TO_RAD = XM_PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / XM_PI;

// ---------- Helper functions ------------
// float2 operators
inline float2 operator+(const float2& a, const float2& b) noexcept
{
    return float2{ a.x + b.x, a.y + b.y };
}

inline float2 operator-(const float2& a, const float2& b) noexcept
{
    return float2{ a.x - b.x, a.y - b.y };
}

inline float2 operator-(const float2& v) noexcept
{
    return float2{ -v.x, -v.y };
}

inline float2 operator*(const float2& v, float s) noexcept
{
    return float2{ v.x * s, v.y * s };
}

inline float2 operator*(float s, const float2& v) noexcept
{
    return v * s;
}

inline float2 operator*(const float2& a, const float2& b) noexcept
{
    return float2{ a.x * b.x, a.y * b.y };
}

inline float2 operator/(const float2& a, const float2& b) noexcept
{
    return float2{ a.x / b.x, a.y / b.y };
}

inline float2 operator/(const float2& v, float s) noexcept
{
    return float2{ v.x / s, v.y / s };
}

inline float2 operator/(float s, const float2& v) noexcept
{
    return float2{ s / v.x, s / v.y };
}
// float3 operators
inline float3 operator+(const float3& a, const float3& b) noexcept
{
    return float3{ a.x + b.x, a.y + b.y, a.z + b.z };
}

inline float3 operator-(const float3& a, const float3& b) noexcept
{
    return float3{ a.x - b.x, a.y - b.y, a.z - b.z };
}

inline float3 operator-(const float3& v) noexcept
{
    return float3{ -v.x, -v.y, -v.z };
}

inline float3 operator*(const float3& v, float s) noexcept
{
    return float3{ v.x * s, v.y * s, v.z * s };
}

inline float3 operator*(float s, const float3& v) noexcept
{
    return v * s;
}

inline float3 operator*(const float3& a, const float3& b) noexcept
{
    return float3{ a.x * b.x, a.y * b.y, a.z * b.z };
}

inline float3 operator/(const float3& a, const float3& b) noexcept
{
    return float3{ a.x / b.x, a.y / b.y, a.z / b.z };
}

inline float3 operator/(const float3& v, float s) noexcept
{
    return float3{ v.x / s, v.y / s, v.z / s };
}

inline float3 operator/(float s, const float3& v) noexcept
{
    return float3{ s / v.x, s / v.y, s / v.z };
}

// float4 operators
inline float4 operator+(const float4& a, const float4& b) noexcept
{
    return float4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

inline float4 operator-(const float4& a, const float4& b) noexcept
{
    return float4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

inline float4 operator-(const float4& v) noexcept
{
    return float4{ -v.x, -v.y, -v.z, -v.w };
}

inline float4 operator*(const float4& v, float s) noexcept
{
    return float4{ v.x * s, v.y * s, v.z * s, v.w * s };
}

inline float4 operator*(float s, const float4& v) noexcept
{
    return v * s;
}

inline float4 operator*(const float4& a, const float4& b) noexcept
{
    return float4{ a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
}

inline float4 operator/(const float4& a, const float4& b) noexcept
{
    return float4{ a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
}

inline float4 operator/(const float4& v, float s) noexcept
{
    return float4{ v.x / s, v.y / s, v.z / s, v.w / s };
}

inline float4 operator/(float s, const float4& v) noexcept
{
    return float4{ s / v.x, s / v.y, s / v.z, s / v.w };
}

// matrix operators
inline float3 operator*(const float3x3& m, const float3& v) noexcept
{
    return float3{
        m._11 * v.x + m._12 * v.y + m._13 * v.z,
        m._21 * v.x + m._22 * v.y + m._23 * v.z,
        m._31 * v.x + m._32 * v.y + m._33 * v.z
    };
}

inline float3 operator*(const float4x3& m, const float3& v) noexcept
{
    return float3{
        m._11 * v.x + m._12 * v.y + m._13 * v.z,
        m._21 * v.x + m._22 * v.y + m._23 * v.z,
        m._31 * v.x + m._32 * v.y + m._33 * v.z,
    };
}

inline float4 operator*(const float4x4& m, const float4& v) noexcept
{
    return float4{
        m._11 * v.x + m._12 * v.y + m._13 * v.z + m._14 * v.w,
        m._21 * v.x + m._22 * v.y + m._23 * v.z + m._24 * v.w,
        m._31 * v.x + m._32 * v.y + m._33 * v.z + m._34 * v.w,
        m._41 * v.x + m._42 * v.y + m._43 * v.z + m._44 * v.w
    };
}

inline float3 operator*(const float3x4& m, const float4& v) noexcept
{
    return float3{
        m._11 * v.x + m._12 * v.y + m._13 * v.z + m._14 * v.w,
        m._21 * v.x + m._22 * v.y + m._23 * v.z + m._24 * v.w,
        m._31 * v.x + m._32 * v.y + m._33 * v.z + m._34 * v.w
    };
}

inline float4x4 operator*(const float4x4& a, const float4x4& b) noexcept
{
    float4x4 result;
    for (size_t row = 0; row < 4; ++row)
    {
        for (size_t col = 0; col < 4; ++col)
        {
            result(row, col) =
                a(row, 0) * b(0, col) +
                a(row, 1) * b(1, col) +
                a(row, 2) * b(2, col) +
                a(row, 3) * b(3, col);
        }
    }
    return result;
}

inline float4x3 operator*(const float4x4& a, const float4x3& b) noexcept
{
    float4x3 result;
    for (size_t row = 0; row < 4; ++row)
    {
        for (size_t col = 0; col < 3; ++col)
        {
            result(row, col) =
                a(row, 0) * b(0, col) +
                a(row, 1) * b(1, col) +
                a(row, 2) * b(2, col) +
                a(row, 3) * b(3, col);
        }
    }
    return result;
}

inline float4x4 Transpose(const float4x4& m) noexcept
{
    return float4x4{
        m._11, m._21, m._31, m._41,
        m._12, m._22, m._32, m._42,
        m._13, m._23, m._33, m._43,
        m._14, m._24, m._34, m._44
    };
}

inline float3 Cross(const float3& a, const float3& b) noexcept
{
    return float3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline float4 Cross(const float4& a, const float4& b) noexcept
{
    return float4{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        0.0f
    };
}

inline float Dot(const float2& a, const float2& b) noexcept
{
    return a.x * b.x + a.y * b.y;
}

inline float Dot(const float3& a, const float3& b) noexcept
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float Dot(const float4& a, const float4& b) noexcept
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline float Length(const float2& v) noexcept
{
    return sqrt(Dot(v, v));
}

inline float Length(const float3& v) noexcept
{
    return sqrt(Dot(v, v));
}

inline float Length(const float4& v) noexcept
{
    return sqrt(Dot(v, v));
}

inline float2 Normalize(const float2& v) noexcept
{
    float len = std::sqrt(Dot(v, v));
    if (len > 0.0f)
    {
        float invLen = 1.0f / len;
        return float2{ v.x * invLen, v.y * invLen };
    }
    return v;
}

inline float3 Normalize(const float3& v) noexcept
{
    float len = Length(v);
    if (len > 0.0f)
    {
        float invLen = 1.0f / len;
        return float3{ v.x * invLen, v.y * invLen, v.z * invLen };
    }
    return v;
}

inline float4 Normalize(const float4& v) noexcept
{
    float len = Length(v);
    if (len > 0.0f)
    {
        float invLen = 1.0f / len;
        return float4{ v.x * invLen, v.y * invLen, v.z * invLen, v.w * invLen};
    }
    return v;
}

inline float Clamp(const float& v, float minVal, float maxVal) noexcept
{
    return std::clamp(v, minVal, maxVal);
}

inline float2 Clamp(const float2& v, float minVal, float maxVal) noexcept
{
    return float2{
        Clamp(v.x, minVal, maxVal),
        Clamp(v.y, minVal, maxVal)
    };
}

inline float3 Clamp(const float3& v, float minVal, float maxVal) noexcept
{
    return float3{
        Clamp(v.x, minVal, maxVal),
        Clamp(v.y, minVal, maxVal),
        Clamp(v.z, minVal, maxVal)
    };
}

inline float4 Clamp(const float4& v, float minVal, float maxVal) noexcept
{
    return float4{
        Clamp(v.x, minVal, maxVal),
        Clamp(v.y, minVal, maxVal),
        Clamp(v.z, minVal, maxVal),
        Clamp(v.w, minVal, maxVal)
    };
}

inline float Saturate(float value) noexcept
{
    return Clamp(value, 0.0f, 1.0f);
}

inline float2 Saturate(const float2& v) noexcept
{
    return Clamp(v, 0.0f, 1.0f);
}

inline float3 Saturate(const float3& v) noexcept
{
    return Clamp(v, 0.0f, 1.0f);
}

inline float4 Saturate(const float4& v) noexcept
{
    return Clamp(v, 0.0f, 1.0f);
}

inline float Lerp(float a, float b, float t) noexcept
{
    return std::lerp(a, b, t);
}

inline float2 Lerp(const float2& a, const float2& b, float t) noexcept
{
    return float2{
        Lerp(a.x, b.x, t),
        Lerp(a.y, b.y, t)
    };
}

inline float3 Lerp(const float3& a, const float3& b, float t) noexcept
{
    return float3{
        Lerp(a.x, b.x, t),
        Lerp(a.y, b.y, t),
        Lerp(a.z, b.z, t)
    };
}

inline float4 Lerp(const float4& a, const float4& b, float t) noexcept
{
    return float4{
        Lerp(a.x, b.x, t),
        Lerp(a.y, b.y, t),
        Lerp(a.z, b.z, t),
        Lerp(a.w, b.w, t)
    };
}

inline float Radians(float degrees) noexcept
{
    return degrees * (std::numbers::pi_v<float> / 180.0f);
}

inline float2 Radians(float2& degrees) noexcept
{
    return degrees * (std::numbers::pi_v<float> / 180.0f);
}

inline float3 Radians(float3& degrees) noexcept
{
    return degrees * (std::numbers::pi_v<float> / 180.0f);
}

inline float4 Radians(float4& degrees) noexcept
{
    return degrees * (std::numbers::pi_v<float> / 180.0f);
}

inline float Degrees(float radians) noexcept
{
    return radians * (180.0f / std::numbers::pi_v<float>);
}

inline float2 Degrees(float2& radians) noexcept
{
    return radians * (180.0f / std::numbers::pi_v<float>);
}

inline float3 Degrees(float3& radians) noexcept
{
    return radians * (180.0f / std::numbers::pi_v<float>);
}

inline float4 Degrees(float4& radians) noexcept
{
    return radians * (180.0f / std::numbers::pi_v<float>);
}

inline float Smoothstep(float a, float b, float x) noexcept
{
    x = Saturate((x - a) / (b - a));
    return x * x * (3 - 2 * x);
}

inline float2 Smoothstep(float a, float b, const float2& x) noexcept
{
    return float2{
        Smoothstep(a, b, x.x),
        Smoothstep(a, b, x.y)
    };
}

inline float3 Smoothstep(float a, float b, const float3& x) noexcept
{
    return float3{
        Smoothstep(a, b, x.x),
        Smoothstep(a, b, x.y),
        Smoothstep(a, b, x.z)
    };
}

inline float4 Smoothstep(float a, float b, const float4& x) noexcept
{
    return float4{
        Smoothstep(a, b, x.x),
        Smoothstep(a, b, x.y),
        Smoothstep(a, b, x.z),
        Smoothstep(a, b, x.w)
    };
}

// Reflect incident vector i around normal n
inline float3 Reflect(const float3& i, const float3& n) noexcept
{
    return i - 2.0f * Dot(n, i) * n;
}

// Reflect incident vector i around normal n
// n.w remains unchanged
inline float4 Reflect(const float4& i, const float4& n) noexcept
{
    float4 result = i - 2.0f * Dot(n, i) * n;
    result.w = n.w;
    return result;
}


inline float Atan(float y) noexcept
{
    return std::atan(y);
}
inline float Atan2(float y, float x) noexcept
{
    return std::atan2(y, x);
}
inline float Acos(float x) noexcept
{
    return std::acos(x);
}
inline float Asin(float x) noexcept
{
    return std::asin(x);
}
inline float Cos(float x) noexcept
{
    return std::cos(x);
}
inline float Sin(float x) noexcept
{
    return std::sin(x);
}
inline float Tan(float x) noexcept
{
    return std::tan(x);
}
inline float Exp(float x) noexcept
{
    return std::exp(x);
}
inline float Log(float x) noexcept
{
    return std::log(x);
}
inline float Pow(float x, float y) noexcept
{
    return std::pow(x, y);
}
inline float Sqrt(float x) noexcept
{
    return std::sqrt(x);
}