#ifndef MESH_H_
#define MESH_H_

/// The following code is adapted from the NCCA NGL AbstractMesh implementation
/// Authors: Jon Macey
/// https://github.com/NCCA/NGL/blob/master/include/ngl/AbstractMesh.h

#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <ngl/Transformation.h>
#include <ngl/AbstractVAO.h>
#include <ngl/SimpleVAO.h>

class Face
{
public :
  Face()=default;
  Face(const Face &_f)=default;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the number of vertices in the face
  //----------------------------------------------------------------------------------------------------------------------
  //size_t m_numVerts=0;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief The vertices index
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<uint32_t> m_vert;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief The texture co-ord index
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<uint32_t> m_uv;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the normal index
  //----------------------------------------------------------------------------------------------------------------------
  std::vector<uint32_t> m_norm;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief debug flag to turn face on and off
  //----------------------------------------------------------------------------------------------------------------------
  bool m_textureCoord=false;
  bool m_normals=false;
};

struct VertData
{
  GLfloat x; // position from obj
  GLfloat y;
  GLfloat z;
  GLfloat nx; // normal from obj mesh
  GLfloat ny;
  GLfloat nz;
  GLfloat u; // tex cords
  GLfloat v; // tex cords
};

class Mesh
{
public:
    Mesh() = default;
    virtual ~Mesh();

    void CreateVAO();
    void BindVAO() const;
    void UnbindVAO() const;
    void Draw() const;

    virtual bool Load(const std::string &_fname) noexcept = 0;

    GLuint getVAOBufferID() {return m_vaoMesh->getBufferID() ;}

    ngl::Vec3 GetPosition() const { return m_transform.getPosition(); }
    ngl::Vec3 GetRotation() const { return m_transform.getRotation(); }
    ngl::Vec3 GetScale() const { return m_transform.getScale(); }

    void SetPosition(const ngl::Vec3& pos) { return m_transform.setPosition(pos); }
    void SetRotation(const ngl::Vec3& rot) { return m_transform.setRotation(rot); }
    void SetScale(const ngl::Vec3& scale) { return m_transform.setScale(scale); }

    ngl::Transformation& GetTransform() { return m_transform; }


protected:

    std::vector<ngl::Vec3> m_verts;
    std::vector<ngl::Vec3> m_norm;
    std::vector<ngl::Vec3> m_uv;
    std::vector<Face> m_face;

    std::unique_ptr<ngl::AbstractVAO> m_vaoMesh;

    GLuint m_dataPackType = GL_TRIANGLES;

    size_t m_meshSize = 0;

    bool m_hasVAO = false;
    bool m_hasVBO = false;
    bool m_loaded = false;

    ngl::Transformation m_transform;
};

#endif