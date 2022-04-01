/// The following code is adapted from the NCCA NGL AbstractMesh implementation
/// Authors: Jon Macey
/// https://github.com/NCCA/NGL/blob/master/src/Obj.cpp

#include "ObjMesh.h"
#include <fstream>
#include <ngl/pystring.h>
#include <iostream>

ObjMesh::ObjMesh(const std::string &_fname) noexcept : Mesh()
  {
    if (Load(_fname) == false)
    {
      ngl::msg->addError(fmt::format("Error loading file {0} ", _fname.data()));
      exit(EXIT_FAILURE);
    }
  }

ObjMesh::ObjMesh(const ObjMesh &_c) noexcept
{
    m_verts = _c.m_verts;
    m_norm = _c.m_norm;
    m_uv = _c.m_uv;
    m_face = _c.m_face;
    //m_center = _c.m_center;
    //m_vbo = false;
    m_hasVAO = false;
    //m_vboMapped = false;
    //m_maxX = _c.m_maxX;
    //m_minX = _c.m_minX;
    //m_maxY = _c.m_maxY;
    //m_minY = _c.m_minY;
    //m_maxZ = _c.m_maxZ;
    //m_minZ = _c.m_minZ;
    //m_loaded = true;
    //m_sphereCenter = _c.m_sphereCenter;
    //m_sphereRadius = _c.m_sphereRadius;
}

void ObjMesh::AddVertex(const ngl::Vec3 &_v) noexcept
{
m_verts.push_back(_v);
}
void ObjMesh::AddNormal(const ngl::Vec3 &_v) noexcept
{
m_norm.push_back(_v);
}
void ObjMesh::AddUV(const ngl::Vec2 &_v) noexcept
{
ngl::Vec3 v(_v.m_x, _v.m_y, 0.0f);
m_uv.push_back(v);
}
void ObjMesh::AddUV(const ngl::Vec3 &_v) noexcept
{
m_uv.push_back(_v);
}

void ObjMesh::AddFace(const Face &_f) noexcept
{
m_face.push_back(_f);
}

// function from https://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
// added after windows build broke NGL tests,
std::istream &safeGetline(std::istream &is, std::string &t)
{
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf *sb = is.rdbuf();

    for (;;)
    {
        int c = sb->sbumpc();
        switch (c)
        {
            case '\n':
            return is;
            case '\r':
            if (sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
            case std::streambuf::traits_type::eof():
            // Also handle the case when the last line has no line ending
            if (t.empty())
                is.setstate(std::ios::eofbit);
            return is;
            default:
            t += static_cast<char>(c);
        }
    }
}

float distSqrd(const ngl::Vec3& a, const ngl::Vec3& b)
{
    return pow( a.m_x - b.m_x, 2 ) + pow( a.m_y - b.m_y, 2 ) + pow( a.m_z - b.m_z, 2 );
}

void ObjMesh::Triangulate()
{
    std::vector<Face> newFaces = m_face;
    std::cout<<m_face.size()<<"\n";
    for(int i = 0; i < m_face.size(); ++i)
    {
        if(m_face[i].m_vert.size() == 4)
        {
            /*float AC, BD;

            AC = distSqrd(m_face[i].m_vert[0], m_face[i].m_vert[2]);
            BD = distSqrd(m_face[i].m_vert[1], m_face[i].m_vert[3]);

            int diagonal = 0;

            if( AC - 0.001f > BD )
                diagonal = 1;

            Face tri1;

            int id0 = diagonal;
            int id1 = (diagonal + 1) % 4;
            int id2 = (diagonal + 2) % 4;
            int id3 = (diagonal + 3) % 4;

            tri1.m_vert = { m_face[i].m_vert[id0], m_face[i].m_vert[id1], m_face[i].m_vert[id2] };
            tri1.m_norm = { m_face[i].m_norm[id0], m_face[i].m_norm[id1], m_face[i].m_norm[id2] };
            tri1.m_uv = { m_face[i].m_uv[id0], m_face[i].m_uv[id1], m_face[i].m_uv[id2] };

            newFaces[i] = tri1;

            Face tri2;

            tri2.m_vert = { m_face[i].m_vert[id2], m_face[i].m_vert[id3], m_face[i].m_vert[id0] };
            tri2.m_norm = { m_face[i].m_norm[id2], m_face[i].m_norm[id3], m_face[i].m_norm[id0] };
            tri2.m_uv = { m_face[i].m_uv[id2], m_face[i].m_uv[id3], m_face[i].m_uv[id0] };

            newFaces.push_back(tri2);*/

            ngl::Vec3 newVert, newNorm, newUV;
            for(int j = 0; j < 4; ++j)
            {
                ngl::Vec3 vert = m_verts[ m_face[i].m_vert[j] ];
                ngl::Vec3 u = m_uv[ m_face[i].m_uv[j] ];
                ngl::Vec3 n = m_norm[ m_face[i].m_norm[j] ];

                newVert += vert;
                newUV += u;
                newNorm += n;
            }
            newVert /= 4.0f;
            newUV /= 4.0f;
            newNorm /= 4.0f;
            newNorm.normalize();
            //std::cout << newNorm.m_x << " " << newNorm.m_y << " " << newNorm.m_z << "\n";
            //newNorm *= -1.0f;
            //newNorm = ngl::Vec3(0, 1, 0);

            m_verts.push_back(newVert);
            m_uv.push_back(newUV);
            m_norm.push_back(newNorm);

            Face tri;
            for(int z = 0; z < 3; ++z)
            {
                tri.m_vert = { m_face[i].m_vert[z], m_face[i].m_vert[z + 1], (unsigned int)m_verts.size() - 1 };
                tri.m_uv = { m_face[i].m_uv[z], m_face[i].m_uv[z + 1], (unsigned int)m_uv.size() - 1 };
                tri.m_norm = { m_face[i].m_norm[z], m_face[i].m_norm[z + 1], (unsigned int)m_norm.size() - 1 };
                newFaces.push_back(tri);
            }
            
            tri.m_vert = { m_face[i].m_vert[3], m_face[i].m_vert[0], (unsigned int)m_verts.size() - 1 };
            tri.m_uv = { m_face[i].m_uv[3], m_face[i].m_uv[0], (unsigned int)m_uv.size() - 1 };
            tri.m_norm = { m_face[i].m_norm[3], m_face[i].m_norm[0], (unsigned int)m_norm.size() - 1 };
            newFaces[i] = tri;
        }
    }
    m_face = newFaces;
    std::cout<<m_face.size()<<"\n";
}

bool ObjMesh::Load(const std::string &_fname) noexcept
{
    std::ifstream in(_fname.data());
    if (in.is_open() != true)
    {
        //msg->addError(fmt::format(" file {0} not found  ", _fname.data()));
        return false;
    }

    std::string str;
    // Read the next line from File untill it reaches the end.
    // while (std::getline(in, str))
    while (!safeGetline(in, str).eof())
    {
        bool status = true;
        // Line contains string of length > 0 then parse it
        if (str.size() > 0)
        {
            std::vector<std::string> tokens;
            pystring::split(str, tokens);
            if (tokens[0] == "v")
            {
                status = ParseVertex(tokens);
            }
            else if (tokens[0] == "vn")
            {
                status = ParseNormal(tokens);
            }

            else if (tokens[0] == "vt")
            {
                status = ParseUV(tokens);
            }
            else if (tokens[0] == "f")
            {
                status = ParseFace(tokens);
            }
        }
        // early out sanity checks!
        if (status == false)
            return false;
    } // while

    in.close();

    m_isLoaded = true;

    Triangulate();

    CreateVAO();
    return true;
}

bool ObjMesh::ParseVertex(std::vector<std::string> &_tokens) noexcept
{
    bool parsedOK = true;
    try
    {
        float x = std::stof(_tokens[1]);
        float y = std::stof(_tokens[2]);
        float z = std::stof(_tokens[3]);
        m_verts.push_back({x, y, z});
        ++m_currentVertexOffset;
    }
    catch (std::invalid_argument &arg)
    {
        ngl::msg->addError("problem converting Obj file vertex ");
        ngl::msg->addError(arg.what());
        parsedOK = false;
    }
    return parsedOK;
}

bool ObjMesh::ParseNormal(std::vector<std::string> &_tokens) noexcept
{
    bool parsedOK = true;
    try
    {
        float x = std::stof(_tokens[1]);
        float y = std::stof(_tokens[2]);
        float z = std::stof(_tokens[3]);
        m_norm.push_back({x, y, z});
        ++m_currentNormalOffset;
    }
    catch (std::invalid_argument &arg)
    {
        ngl::msg->addError("problem converting Obj file normals");
        ngl::msg->addError(arg.what());
        parsedOK = false;
    }
    return parsedOK;
}

bool ObjMesh::ParseUV(std::vector<std::string> &_tokens) noexcept
{
    bool parsedOK = true;
    try
    {
        float x, y, z = 0.0f;
        x = std::stof(_tokens[1]);
        y = std::stof(_tokens[2]);
        if (_tokens.size() == 4)
        {
        z = std::stof(_tokens[3]);
        }

        m_uv.push_back({x, y, z});
        ++m_currentUVOffset;
    }
    catch (std::invalid_argument &arg)
    {
        ngl::msg->addError("problem converting Obj file UV's");
        ngl::msg->addError(arg.what());

        parsedOK = false;
    }
    return parsedOK;
}

bool ObjMesh::ParseFace(std::vector<std::string> &_tokens) noexcept
{
    bool parsedOK = true;
    // first let's find what sort of face we are dealing with
    // I assume most likely case is all
    if (pystring::count(_tokens[1], "/") == 2 && pystring::find(_tokens[1], "//") == -1)
    {
        parsedOK = ParseFaceVertexNormalUV(_tokens);
    }

    else if (pystring::find(_tokens[1], "/") == -1)
    {
        parsedOK = ParseFaceVertex(_tokens);
    }
    // look for VertNormal
    else if (pystring::find(_tokens[1], "//") != -1)
    {
        parsedOK = ParseFaceVertexNormal(_tokens);
    }
    // if we have 1 / it is a VertUV format
    else if (pystring::count(_tokens[1], "/") == 1)
    {
        parsedOK = ParseFaceVertexUV(_tokens);
    }

    return parsedOK;

}
// f v v v v
bool ObjMesh::ParseFaceVertex(std::vector<std::string> &_tokens) noexcept
{
    bool parsedOK = true;
    Face f;
    // token still starts with f so skip
    for (size_t i = 1; i < _tokens.size(); ++i)
    {
        try
        {
            // note we need to subtract one from the list
            int idx = std::stoi(_tokens[i]) - 1;
            // check if we are a negative index
            if (std::signbit(idx))
            {
                // note we index from 0 not 1 like obj so adjust
                idx = m_currentVertexOffset + (idx + 1);
            }
            f.m_vert.push_back(static_cast<uint32_t>(idx));
        }
        catch (std::invalid_argument &arg)
        {
            ngl::msg->addError("problem converting Obj file face");
            ngl::msg->addError(arg.what());
            parsedOK = false;
        }
    }
    m_face.push_back(f);
    return parsedOK;
}
// f v//vn v//vn v//vn v//vn
bool ObjMesh::ParseFaceVertexNormal(std::vector<std::string> &_tokens) noexcept
{
    bool parsedOK = true;
    Face f;
    // token still starts with f so skip
    for (size_t i = 1; i < _tokens.size(); ++i)
    {
        std::vector<std::string> vn;
        pystring::split(_tokens[i], vn, "//");
        try
        {
            // note we need to subtract one from the list
            int idx = std::stoi(vn[0]) - 1;
            // check if we are a negative index
            if (std::signbit(idx))
            {
                // note we index from 0 not 1 like obj so adjust
                idx = m_currentVertexOffset + (idx + 1);
            }
            f.m_vert.push_back(static_cast<uint32_t>(idx));
            idx = std::stoi(vn[1]) - 1;
            // check if we are a negative index
            if (std::signbit(idx))
            {
                // note we index from 0 not 1 like obj so adjust
                idx = m_currentNormalOffset + (idx + 1);
            }
            f.m_norm.push_back(static_cast<uint32_t>(idx));
        }
        catch (std::invalid_argument &arg)
        {
        ngl::msg->addError("problem converting Obj file face");
        ngl::msg->addError(arg.what());
        parsedOK = false;
        }
    }
    m_face.push_back(f);
    return parsedOK;
}
// f v/vt v/vt v/vt v/vt
bool ObjMesh::ParseFaceVertexUV(std::vector<std::string> &_tokens) noexcept
{
    bool parsedOK = true;
    Face f;
    // token still starts with f so skip
    for (size_t i = 1; i < _tokens.size(); ++i)
    {
        std::vector<std::string> vn;
        pystring::split(_tokens[i], vn, "/");
        try
        {
            // note we need to subtract one from the list
            int idx = std::stoi(vn[0]) - 1;
            // check if we are a negative index
            if (std::signbit(idx))
            {
                // note we index from 0 not 1 like obj so adjust
                idx = m_currentVertexOffset + (idx + 1);
            }
            f.m_vert.push_back(static_cast<uint32_t>(idx));
            idx = std::stoi(vn[1]) - 1;
            // check if we are a negative index
            if (std::signbit(idx))
            {
                // note we index from 0 not 1 like obj so adjust
                idx = m_currentUVOffset + (idx + 1);
            }
            f.m_uv.push_back(static_cast<uint32_t>(idx));
        }
        catch (std::invalid_argument &arg)
        {
            ngl::msg->addError("problem converting Obj file face");
            ngl::msg->addError(arg.what());
            parsedOK = false;
        }
    }
    m_face.push_back(f);
    return parsedOK;
}
// f v/vt/vn v/vt/vn v/vt/vn v/vt/vnm_face
bool ObjMesh::ParseFaceVertexNormalUV(std::vector<std::string> &_tokens) noexcept
{
    bool parsedOK = true;
    Face f;
    for (size_t i = 1; i < _tokens.size(); ++i)
    {
        std::vector<std::string> vn;
        pystring::split(_tokens[i], vn, "/");
        try
        {
            // note we need to subtract one from the list
            int idx = std::stoi(vn[0]) - 1;
            // check if we are a negative index
            if (std::signbit(idx))
            {
                // note we index from 0 not 1 like obj so adjust
                idx = m_currentVertexOffset + (idx + 1);
            }
            f.m_vert.push_back(static_cast<uint32_t>(idx));

            idx = std::stoi(vn[1]) - 1;
            // check if we are a negative index
            if (std::signbit(idx))
            {
                // note we index from 0 not 1 like obj so adjust
                idx = m_currentUVOffset + (idx + 1);
            }
            f.m_uv.push_back(static_cast<uint32_t>(idx));

            idx = std::stoi(vn[2]) - 1;
            // check if we are a negative index
            if (std::signbit(idx))
            {
                // note we index from 0 not 1 like obj so adjust
                idx = m_currentNormalOffset + (idx + 1);
            }
            f.m_norm.push_back(static_cast<uint32_t>(idx));
        }
        catch (std::invalid_argument &arg)
        {
            ngl::msg->addError("problem converting Obj file face");
            ngl::msg->addError(arg.what());
            parsedOK = false;
        }
    }
    m_face.push_back(f);
    return parsedOK;
}