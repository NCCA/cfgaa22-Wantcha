#ifndef MESH_H_
#define MESH_H_

/// The following code is adapted from the NCCA NGL AbstractMesh implementation
/// Authors: Jon Macey
/// https://github.com/NCCA/NGL/blob/master/include/ngl/AbstractMesh.h

#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <ngl/Vec3.h>
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
  size_t m_numVerts=0;
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
};

#endif