#ifndef OBJMESH_H_
#define OBJMESH_H_

/// The following code is adapted from the NCCA NGL AbstractMesh implementation
/// Authors: Jon Macey
/// https://github.com/NCCA/NGL/blob/master/include/ngl/Obj.h

#include "Mesh.h"
#include <ngl/Vec2.h>
#include "Assets/Asset.h"

class ObjMesh : public Mesh
{
public:
    ObjMesh()  noexcept: Mesh(){}

    ObjMesh( const std::string& _fname)  noexcept ;

    ObjMesh(const ObjMesh &_c) noexcept;

    void AddVertex(const ngl::Vec3 &_v) noexcept;
    void AddNormal(const ngl::Vec3 &_v) noexcept;
    void AddUV(const ngl::Vec2 &_v) noexcept;
    void AddUV(const ngl::Vec3 &_v) noexcept;
    void AddFace(const Face &_f) noexcept;

    virtual bool Load(const std::string &_fname) noexcept;

    bool IsLoaded() const noexcept {return m_isLoaded;}

    //ASSET_CLASS(AssetType::Mesh)
private:
    void Triangulate();

    bool ParseVertex(std::vector<std::string> &_tokens) noexcept;
    bool ParseNormal(std::vector<std::string> &_tokens) noexcept;
    bool ParseUV(std::vector<std::string> &_tokens) noexcept;
    // face parsing is complex we have different layouts.
    // don't forget we can also have negative indices
    bool ParseFace(std::vector<std::string> &_tokens) noexcept;
    // f v v v v
    bool ParseFaceVertex(std::vector<std::string> &_tokens) noexcept;
    // f v//vn v//vn v//vn v//vn
    bool ParseFaceVertexNormal(std::vector<std::string> &_tokens) noexcept;
    // f v/vt v/vt v/vt v/vt
    bool ParseFaceVertexUV(std::vector<std::string> &_tokens) noexcept;
    // f v/vt/vn v/vt/vn v/vt/vn v/vt/vn
    bool ParseFaceVertexNormalUV(std::vector<std::string> &_tokens) noexcept;
    // as faces can use negative index values keep track of index
    int m_currentVertexOffset=0;
    int m_currentNormalOffset=0;
    int m_currentUVOffset=0;

    bool m_isLoaded = false;
};

#endif