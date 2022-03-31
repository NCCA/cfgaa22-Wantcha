/// The following code is adapted from the NCCA NGL AbstractMesh implementation
/// Authors: Jon Macey
/// https://github.com/NCCA/NGL/blob/master/src/AbstractMesh.cpp

#include "Mesh.h"
#include <ngl/VAOFactory.h>
#include <iostream>

Mesh::~Mesh() noexcept
{
  if(m_loaded == true)
  {
    m_verts.erase(m_verts.begin(),m_verts.end());
    m_norm.erase(m_norm.begin(),m_norm.end());
    m_uv.erase(m_uv.begin(),m_uv.end());
    m_face.erase(m_face.begin(),m_face.end());

    /*if(m_hasVBO)
    {
      glDeleteBuffers(1,&m_vboBuffers);
    }*/
  }
}

void Mesh::CreateVAO()
{
    std::vector <VertData> vboMesh;
    VertData d;

	// loop for each of the faces
    for(unsigned int i=0;i<m_face.size();++i)
    {
        for(unsigned int j=0;j<3;++j)
        {

            // pack in the vertex data first
            d.x = m_verts[m_face[i].m_vert[j]].m_x;
            d.y = m_verts[m_face[i].m_vert[j]].m_y;
            d.z = m_verts[m_face[i].m_vert[j]].m_z;
            // now if we have norms of tex (possibly could not) pack them as well
            if(m_norm.size() >0 && m_uv.size() > 0)
            {
                d.nx = m_norm[m_face[i].m_norm[j]].m_x;
                d.ny = m_norm[m_face[i].m_norm[j]].m_y;
                d.nz = m_norm[m_face[i].m_norm[j]].m_z;

                d.u = m_uv[m_face[i].m_uv[j]].m_x;
                d.v = m_uv[m_face[i].m_uv[j]].m_y;
            }
            // now if neither are present (only verts like Zbrush models)
            else if(m_norm.size() ==0 && m_uv.size()==0)
            {
                d.nx = 0;
                d.ny = 0;
                d.nz = 0;
                d.u = 0;
                d.v = 0;
            }
            // here we've got norms but not tex
            else if(m_norm.size() >0 && m_uv.size()==0)
            {
                d.nx = m_norm[m_face[i].m_norm[j]].m_x;
                d.ny = m_norm[m_face[i].m_norm[j]].m_y;
                d.nz = m_norm[m_face[i].m_norm[j]].m_z;
                d.u = 0;
                d.v = 0;
            }
            // here we've got tex but not norm least common
            else if(m_norm.size() ==0 && m_uv.size()>0)
            {
                d.nx = 0;
                d.ny = 0;
                d.nz = 0;
                d.u = m_uv[m_face[i].m_uv[j]].m_x;
                d.v = m_uv[m_face[i].m_uv[j]].m_y;
            }
            vboMesh.push_back(d);
        }
    }
    m_vaoMesh = ngl::VAOFactory::createVAO(ngl::simpleVAO, m_dataPackType);
    m_vaoMesh->bind();
    m_meshSize = vboMesh.size();

    m_vaoMesh->setData(ngl::SimpleVAO::VertexData(m_meshSize*sizeof(VertData), vboMesh[0].x));

    m_vaoMesh->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(VertData), 0);

    m_vaoMesh->setVertexAttributePointer(1, 3, GL_FLOAT, sizeof(VertData), 3);

    m_vaoMesh->setVertexAttributePointer(2, 2, GL_FLOAT, sizeof(VertData), 6);

    m_vaoMesh->setNumIndices(m_meshSize);

    m_hasVAO = true;


}

void Mesh::BindVAO() const
{
    m_vaoMesh->bind();
}

void Mesh::UnbindVAO() const
{
    m_vaoMesh->unbind();
}

void Mesh::Draw() const
{
    if(m_hasVAO)
    {
        m_vaoMesh->bind();
        m_vaoMesh->draw();
        m_vaoMesh->unbind();
    }
}