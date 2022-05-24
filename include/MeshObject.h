#ifndef MESH_OBJECT_H_
#define MESH_OBJECT_H_

#include "SceneObject.h"

class MeshObject : public SceneObject
{
public:
    MeshObject() = default;
    MeshObject(const std::string& filepath);
    virtual ~MeshObject() = default;

    //virtual void Draw() const override { m_mesh->Draw(); }
    //virtual void DrawHighlighted() const override { m_mesh->DrawWireframe(); }
    virtual QGridLayout* GetLayout() override;
};

#endif