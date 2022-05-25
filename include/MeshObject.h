#ifndef MESH_OBJECT_H_
#define MESH_OBJECT_H_

#include "SceneObject.h"
#include <string>

class MeshObject : public SceneObject
{
public:
    MeshObject() = default;
    MeshObject(const std::string& filepath);
    virtual ~MeshObject() = default;

    //virtual void Draw() const override { m_mesh->Draw(); }
    //virtual void DrawHighlighted() const override { m_mesh->DrawWireframe(); }
    virtual QGridLayout* GetLayout() override;
public slots:
    void OnChangeAlbedoTexture(const std::string& path);
    void OnChangeRoughnessTexture(const std::string& path);
    void OnChangeNormalTexture(const std::string& path);
    void OnChangeAOTexture(const std::string& path);
    void OnChangeMetallicTexture(const std::string& path);
};

#endif