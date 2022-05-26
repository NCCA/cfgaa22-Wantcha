#ifndef SCENE_OBJECT_H_
#define SCENE_OBJECT_H_

#include "Transform.h"
#include "Mesh.h"
#include "QGridLayout"
#include <QLabel>
#include <QPushButton>
#include <QGraphicsView>
class SceneObject
{
public:
    SceneObject() = default;
    SceneObject(const std::string& filepath);
    virtual ~SceneObject() {}

    ngl::Vec3 GetPosition() const { return m_transform.getPosition(); }
    ngl::Vec3 GetRotation() const { return m_transform.getRotation(); }
    ngl::Vec3 GetScale() const { return m_transform.getScale(); }

    void SetPosition(const ngl::Vec3& pos) { m_transform.setPosition(pos); }
    void SetRotation(const ngl::Vec3& rot) { m_transform.setRotation(rot); }
    void SetScale(const ngl::Vec3& scale) { m_transform.setScale(scale); }

    void SetName(const std::string& name) { m_name = name; }

    Transform& GetTransform() { return m_transform; }
    virtual std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }
    std::string GetName() const { return m_name; }
    bool IsLight() const { return m_isLight; }
    virtual void Draw();
    virtual void DrawHighlighted();
    virtual QGridLayout* GetLayout();
protected:
    Transform m_transform;
    std::string m_name = "Object";
    std::shared_ptr<Mesh> m_mesh;
    bool m_isLight = false;
};

#endif