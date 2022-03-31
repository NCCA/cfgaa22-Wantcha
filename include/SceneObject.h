#ifndef SCENE_OBJECT_H_
#define SCENE_OBJECT_H_

#include <ngl/Transformation.h>

class SceneObject
{
public:
    SceneObject() = default;
    virtual ~SceneObject() {}

    ngl::Vec3 GetPosition() const { return m_transform.getPosition(); }
    //ngl::Vec3 GetRotation() const { return m_transform.getRosition(); }
    //ngl::Vec3 GetPosition() const { return m_transform.getPosition(); }

protected:
    ngl::Transformation m_transform;

};

#endif