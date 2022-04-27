#include "Gizmos.h"

Gizmo::Gizmo()
{
    for(int i = 0 ; i < 3; ++i)
    {
        m_translateMeshes[i] = std::make_unique<Mesh>();
    }
    
    std::vector<ngl::Vec3> linePoints;
    linePoints.push_back( ngl::Vec3{0, 0, 0});
    linePoints.push_back( ngl::Vec3{1, 0, 0});
    m_translateMeshes[0]->SetVertices(linePoints);

    linePoints[1] = ngl::Vec3{0, 1, 0};
    m_translateMeshes[1]->SetVertices(linePoints);

    linePoints[1] = ngl::Vec3{0, 0, 1};
    m_translateMeshes[2]->SetVertices(linePoints);

    m_colors[0] = {1, 0 , 0, 1};
    m_colors[1] = {0, 1 , 0, 1};
    m_colors[2] = {0, 0 , 1, 1};

    ngl::ShaderLib::loadShader("UnlitConstantSize", "shaders/UnlitVert.glsl", "shaders/UnlitFrag.glsl");
    //ngl::ShaderLib::use(m_name);
}

void Gizmo::Draw(const ngl::Mat4& vp, int windowWidth)
{
    //m_transform.setScale(ngl::Vec3{1.0f,1.0f,1.0f});

    ngl::ShaderLib::use("UnlitConstantSize");
    glDisable(GL_DEPTH_TEST);
    ngl::ShaderLib::setUniform("MVP", vp * m_transform.getMatrix());
    ngl::ShaderLib::setUniform("reciprScaleOnScreen", 2.0f * m_sizeOnScreen / windowWidth);
    switch (m_type){
        case GizmoType::TRANSLATE:
            glLineWidth(7);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            for(int i = 0 ; i < 3; ++i)
            {
                if(m_selectedAxis == i)
                {
                    ngl::ShaderLib::setUniform("Color", ngl::Vec4{0.9f, 0.9f, 0.7f, 1.0f});
                }
                else
                {
                    ngl::ShaderLib::setUniform("Color", m_colors[i]);
                }
                
                ngl::ShaderLib::setUniform("objectID", -(i + 1));
                m_translateMeshes[i]->DrawWireframe();
            }
            
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glLineWidth(1);
    }
    glEnable(GL_DEPTH_TEST); 
        
}

void Gizmo::Manipulate(ngl::Transformation& objectTransform, float dx, float dy)
{

    switch (m_type){
        case GizmoType::TRANSLATE:
            ngl::Vec3 moveDirection;
            float axis;
            switch (m_selectedAxis)
            {
                case 0:
                    moveDirection = objectTransform.getMatrix().getRightVector();
                    axis = dx;
                    break;
                case 1:
                    moveDirection = objectTransform.getMatrix().getDownVector();
                    axis = dy;
                    break;
                case 2:
                    moveDirection = objectTransform.getMatrix().getForwardVector();
                    axis = dx;
                    break;
            }

            objectTransform.addPosition( moveDirection * axis * 0.005f );
            break;
    }
}