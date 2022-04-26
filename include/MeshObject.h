#ifndef MESH_OBJECT_H_
#define MESH_OBJECT_H_

#include "Mesh.h"
#include "SceneObject.h"

class MeshObject : public SceneObject
{
public:
    MeshObject() = default;
    MeshObject(const std::string& filepath);
    virtual ~MeshObject() = default;

    virtual void Draw() const override { m_mesh->Draw(); }
    virtual void DrawHighlighted() const override { m_mesh->DrawWireframe(); }
    std::shared_ptr<Mesh> GetMesh() const {return m_mesh; }

private:

    std::shared_ptr<Mesh> m_mesh;
};

#endif