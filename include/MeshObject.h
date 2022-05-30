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

    //virtual QGridLayout* GetLayout() override;
    //virtual void BuildLayout() override;
/*public slots:
    void OnChangeAlbedoTexture(const std::string& path);
    void OnChangeRoughnessTexture(const std::string& path);
    void OnChangeNormalTexture(const std::string& path);
    void OnChangeAOTexture(const std::string& path);
    void OnChangeMetallicTexture(const std::string& path);*/
private:

};

#endif