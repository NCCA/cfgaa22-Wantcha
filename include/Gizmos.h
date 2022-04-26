#ifndef GIZMOS_H_
#define GIZMOS_H_

#include "MeshObject.h"
#include <ngl/Transformation.h>

enum class GizmoType
{
    NONE,
    TRANSLATE,
    ROTATE,
    SCALE
};

class Gizmo
{
public:
    Gizmo();
    ~Gizmo() = default;

    void SetActive(bool active) { m_isActive = active; }
    bool GetActive() const { return m_isActive; }

    void SetTransform(const ngl::Transformation& transform) { m_transform = transform; }

    void SetType(GizmoType type) { m_type = type; }
    GizmoType GetType() const { return m_type; }

    void Draw(const ngl::Mat4& mvp);


private:
    void DrawLine(const ngl::Vec3& p1, const ngl::Vec3& p2);

    bool m_isActive = false;
    GizmoType m_type = GizmoType::TRANSLATE;

    ngl::Transformation m_transform;

    std::unique_ptr<Mesh> m_translateMesh;
    std::unique_ptr<Mesh> m_scaleMesh;
};

#endif