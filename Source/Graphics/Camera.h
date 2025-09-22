#pragma once
#include <DirectXMath.h>
#include "Types/BaseTypes.h"

class Camera
{
public:
    Camera()
        : m_position(0.0f, 0.0f, -5.0f),
        m_target(0.0f, 0.0f, 0.0f),
        m_up(0.0f, 1.0f, 0.0f),
        m_fovY(QUARTER_PI),
        m_aspect(1.0f),
        m_nearZ(0.1f),
        m_farZ(100.0f)
    {
    }

    void SetPosition(const float3& pos) { m_position = pos; }
    void SetTarget(const float3& target) { m_target = target; }
    void SetUp(const float3& up) { m_up = up; }
    void SetPerspective(float fovH, float aspect, float nearZ, float farZ)
    {
        fovH = clamp(fovH, 60, 120) * XM_PI / 180.0f; // Clamp horizontal FOV to [60,120] degrees
        m_fovY = 2 * atan(tan(fovH / 2.0f) * (1.0f / aspect));
        m_aspect = aspect;
        m_nearZ = nearZ;
        m_farZ = farZ;
    }

    float3 GetPosition() const { return m_position; }
    float3 GetTarget() const { return m_target; }
    float3 GetUp() const { return m_up; }

    XMMATRIX GetViewMatrix() const
    {
        return XMMatrixLookAtLH(
            XMLoadFloat3(&m_position),
            XMLoadFloat3(&m_target),
            XMLoadFloat3(&m_up)
        );
    }

    XMMATRIX GetProjectionMatrix() const
    {
        return XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_farZ);
    }

private:
    float3 m_position;
    float3 m_target;
    float3 m_up;
    float m_fovY;
    float m_aspect;
    float m_nearZ;
    float m_farZ;
};