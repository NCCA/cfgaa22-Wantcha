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

    void SetUsing(bool isUsing) { m_isUsed = isUsing; }
    bool GetUsing() const { return m_isUsed; }

    void SetTransform(const ngl::Transformation& transform) { m_transform = transform; }
    void SetSelectedAxis(int axis) { m_selectedAxis = axis; }

    void SetType(GizmoType type) { m_type = type; }
    GizmoType GetType() const { return m_type; }

    void Draw(const ngl::Mat4& vp, int windowWidth);

    void Manipulate(ngl::Transformation& objectTransform, float dx, float dy);


private:
    void DrawLine(const ngl::Vec3& p1, const ngl::Vec3& p2);

    bool m_isActive = false;
    bool m_isUsed = false;

    int m_selectedAxis = -1;
    GizmoType m_type = GizmoType::TRANSLATE;

    ngl::Transformation m_transform;

    std::array<std::unique_ptr<Mesh> , 3> m_translateMeshes;
    std::array<ngl::Vec4, 3> m_colors;
    std::unique_ptr<Mesh> m_scaleMesh;

    float m_sizeOnScreen = 100.0f;
};

#endif