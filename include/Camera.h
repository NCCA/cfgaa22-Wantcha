#ifndef CAMERA_H
#define CAMERA_H

#include <ngl/Mat4.h>
#include <ngl/Vec3.h>
#include <ngl/Quaternion.h>
#include <ngl/Transformation.h>

class Camera
{
public:
    Camera() = default;
    Camera(float fov, float aspectRatio, float nearPlane, float farPlane);

    void SetViewportSize(float width, float height);
    void SetDistance(float distance);
    void GetDistance() const;

    ngl::Mat4 GetViewProjection();
    ngl::Mat4 GetView();
    ngl::Mat4 GetProjection();

    void MouseZoom(float delta);
    void MousePan(const ngl::Vec2& delta);
    void MouseRotate(const ngl::Vec2& delta);

    ~Camera() = default;

private:
    void UpdateProjection();
    void UpdateView();

    ngl::Vec3 CalculatePosition() const;
    ngl::Vec3 GetForward() const;
    ngl::Vec3 GetRight() const;
    ngl::Vec3 GetUp() const;
    ngl::Quaternion GetOrientation() const;

    float m_FOV = 45.0f;
    float m_aspectRatio = 1.77778f;
    float m_nearClip = 0.01f;
    float m_farClip = 1000.0f;

    ngl::Mat4 m_projection;
    ngl::Mat4 m_view;
    ngl::Transformation m_transform;
    ngl::Vec3 m_position = ngl::Vec3::zero();
    ngl::Vec3 m_focalPoint = ngl::Vec3::zero();

    float m_distance = 2.0f;
    float m_pitch = 0.0f, m_yaw = 0.0f;
    float m_width = 1280;
    float m_height = 720;

    float m_zoomSpeed = 0.002f;
    float m_rotationSpeed = 0.175f;
    float m_panSpeed = 0.00175f;
};

#endif