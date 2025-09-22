#pragma once
#include "Engine/BaseTypes.h"

class Camera
{
public:
    Camera()
        : m_Position(0.0f, 0.0f, -5.0f),
        m_Target(0.0f, 0.0f, 0.0f),
        m_Up(0.0f, 1.0f, 0.0f),
        m_FovY(QUARTER_PI),
        m_Aspect(1.0f),
        m_NearZ(0.1f),
        m_FarZ(100.0f)
    {
    }

    void SetPosition(const float3& pos) { m_Position = pos; }
    void SetTarget(const float3& target) { m_Target = target; }
    void SetUp(const float3& up) { m_Up = up; }
    void SetPerspective(float fovH, float aspect, float nearZ, float farZ)
    {
        fovH = Clamp(fovH, 60, 120) * DEG_TO_RAD; // Clamp horizontal FOV to [60,120] degrees
        m_FovY = 2 * Atan(Tan(fovH / 2.0f) * (1.0f / aspect));
        m_Aspect = aspect;
        m_NearZ = nearZ;
        m_FarZ = farZ;
    }

    const float3& GetPosition() const { return m_Position; }
    const float3& GetTarget() const { return m_Target; }
    const float3& GetUp() const { return m_Up; }
    const float& GetFovY() const { return m_FovY; }
    const float& GetAspect() const { return m_Aspect; }
    const float& GetNearZ() const { return m_NearZ; }
    const float& GetFarZ() const { return m_FarZ; }

private:
    float3 m_Position;
    float3 m_Target;
    float3 m_Up;
    float m_FovY;
    float m_Aspect;
    float m_NearZ;
    float m_FarZ;
};