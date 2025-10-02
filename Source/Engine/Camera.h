#pragma once
#include "Engine/BaseTypes.h"

class Camera
{
public:
    Camera()
        : m_Position(0.0f, 0.0f, -5.0f),
        m_Forward(0.0f, 0.0f, 1.0f),
        m_Up(0.0f, 1.0f, 0.0f),
        m_FovY(QUARTER_PI),
        m_Aspect(1.0f),
        m_NearZ(0.1f),
        m_FarZ(100.0f)
    {
    }

    void SetPosition(const float3& pos) { m_Position = pos; }
    void SetForward(const float3& target) { m_Forward = target; }
    void SetUp(const float3& up) { m_Up = up; }
    void SetPerspective(float fovH, float aspect, float nearZ, float farZ)
    {
        fovH = Clamp(fovH, 60, 120) * DEG_TO_RAD * 0.5; // Clamp horizontal FOV to [60,120] degrees
        m_FovY = 2 * Atan(Tan(fovH / 2.0f) * (1.0f / aspect));
        m_Aspect = aspect;
        m_NearZ = nearZ;
        m_FarZ = farZ;
    }

    const float3& GetPosition() const { return m_Position; }
    const float3& GetForward() const { return m_Forward; }
    const float3& GetUp() const { return m_Up; }
    const float& GetFovY() const { return m_FovY; }
    const float& GetAspect() const { return m_Aspect; }
    const float& GetNearZ() const { return m_NearZ; }
    const float& GetFarZ() const { return m_FarZ; }


    inline XMMATRIX GetViewMatrix() const
    {
        float3 target = m_Position + m_Forward;
        return XMMatrixLookAtLH(
            XMLoadFloat3(&m_Position),
            XMLoadFloat3(&target),
            XMLoadFloat3(&m_Up)
        );
    }

    inline XMMATRIX GetProjectionMatrix() const
    {
        return XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
    }

    inline XMMATRIX GetViewProjectionMatrix() const
    {
        return GetViewMatrix() * GetProjectionMatrix();
    }

private:
    float3 m_Position;
    float3 m_Forward;
    float3 m_Up;
    float m_FovY;
    float m_Aspect;
    float m_NearZ;
    float m_FarZ;
};