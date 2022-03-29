#include "Camera.h"

#include <ngl/ShaderLib.h>

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
    : m_FOV(fov), m_aspectRatio(aspectRatio), m_nearClip(nearPlane), m_farClip(farPlane)

{
    UpdateView();
    UpdateProjection();
}

void Camera::SetViewportSize(float width, float height)
{
    m_width = width;
    m_height = height;
    UpdateProjection();
}

void Camera::UpdateProjection()
{
    m_aspectRatio = m_width / m_height;
	m_projection = ngl::perspective(m_FOV, m_aspectRatio, m_nearClip, m_farClip);
    
}

void Camera::UpdateView()
{
    m_position = CalculatePosition();

    m_transform = ngl::Transformation();
    m_transform.addPosition(m_position);
    m_transform.addRotation(ngl::Vec3(-m_pitch, -m_yaw, 0.0f));
    m_view = m_transform.getMatrix().inverse();
}

void Camera::MouseZoom(float delta)
{
    m_distance -= delta * m_zoomSpeed;
    UpdateView();
    //std::cout<<"aa";
}

void Camera::MousePan(const ngl::Vec2& delta)
{
    m_focalPoint += -GetRight() * delta.m_x * m_panSpeed * m_distance;
    m_focalPoint += GetUp() * delta.m_y * m_panSpeed * m_distance;
    UpdateView();
}

void Camera::MouseRotate(const ngl::Vec2& delta)
{
    m_yaw += delta.m_x * m_rotationSpeed;
    m_pitch += delta.m_y * m_rotationSpeed;
    UpdateView();
}

ngl::Mat4 Camera::GetViewProjection()
{
    return m_projection * m_view;
}

ngl::Mat4 Camera::GetView()
{
    return m_view;
}

ngl::Mat4 Camera::GetProjection()
{
    return m_projection;
}

ngl::Vec3 Camera::CalculatePosition() const
{
    return m_focalPoint - GetForward() * m_distance;
}

ngl::Vec3 Camera::GetForward() const
{
    return GetOrientation().toMat4().getForwardVector();
}

ngl::Vec3 Camera::GetRight() const
{
    return GetOrientation().toMat4().getRightVector();
}

ngl::Vec3 Camera::GetUp() const
{
    return GetOrientation().toMat4().getUpVector();
}

ngl::Quaternion Camera::GetOrientation() const
{
    return ngl::Quaternion(ngl::Vec3(-m_pitch, -m_yaw, 0.0f));
}
