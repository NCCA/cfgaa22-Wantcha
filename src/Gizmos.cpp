#include "Gizmos.h"

Gizmo::Gizmo()
{
    m_translateMesh = std::make_unique<Mesh>();
    std::vector<ngl::Vec3> linePoints;
    linePoints.push_back( ngl::Vec3{0, 0, 0});
    linePoints.push_back( ngl::Vec3{0, 0, 1});
    m_translateMesh->SetVertices(linePoints);
}

void Gizmo::Draw(const ngl::Mat4& vp)
{
    glDisable(GL_DEPTH_TEST);
    ngl::ShaderLib::use(ngl::nglColourShader);
    ngl::ShaderLib::setUniform("MVP", vp * m_transform.getMatrix());
    ngl::ShaderLib::setUniform("Colour", ngl::Vec4(1.0f, 0.7f, 0.45f, 1.0f));

    switch (m_type){
        case GizmoType::TRANSLATE:
            glLineWidth(5);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            m_translateMesh->DrawWireframe();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glLineWidth(1);
    }
    glEnable(GL_DEPTH_TEST); 
        
}