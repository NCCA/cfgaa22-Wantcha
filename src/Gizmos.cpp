#include "Gizmos.h"

Gizmo::Gizmo()
{
    for(int i = 0 ; i < 3; ++i)
    {
        m_translateMeshes[i] = std::make_unique<Mesh>(GL_TRIANGLES);
        m_rotateMeshes[i] = std::make_unique<Mesh>(GL_TRIANGLES);
        m_scaleMeshes[i] = std::make_unique<Mesh>(GL_TRIANGLES);
    }
    
    std::vector<ngl::Vec3> linePoints =
                {ngl::Vec3{0, m_lineWidth / 2.0f, 0}, ngl::Vec3{0.9f, m_lineWidth / 2.0f, 0}, ngl::Vec3{0.9f, m_lineWidth, 0}, ngl::Vec3{1, 0, 0},
                ngl::Vec3{0.9f, -m_lineWidth, 0}, ngl::Vec3{0.9f, -m_lineWidth / 2.0f, 0}, ngl::Vec3{0.0f, -m_lineWidth / 2.0f, 0} };

    std::vector<ngl::Vec3> triangleVerts = 
                    { linePoints[0], linePoints[6], linePoints[1],
                    linePoints[1], linePoints[3], linePoints[2],
                    linePoints[3], linePoints[1], linePoints[5],
                    linePoints[4], linePoints[3], linePoints[5],
                    linePoints[6], linePoints[5], linePoints[1]};
    m_translateMeshes[0]->SetVertices(triangleVerts);

    linePoints =
                {ngl::Vec3{m_lineWidth / 2.0f, 0, 0}, ngl::Vec3{m_lineWidth / 2.0f, 0.9f, 0}, ngl::Vec3{m_lineWidth, 0.9f,0}, ngl::Vec3{0, 1, 0},
                ngl::Vec3{-m_lineWidth, 0.9f,0}, ngl::Vec3{-m_lineWidth / 2.0f, 0.9f, 0}, ngl::Vec3{-m_lineWidth / 2.0f, 0, 0}};
    triangleVerts = 
                    { linePoints[0], linePoints[6], linePoints[1],
                    linePoints[1], linePoints[3], linePoints[2],
                    linePoints[3], linePoints[1], linePoints[5],
                    linePoints[4], linePoints[3], linePoints[5],
                    linePoints[6], linePoints[5], linePoints[1]};
    m_translateMeshes[1]->SetVertices(triangleVerts);

    linePoints =
                {ngl::Vec3{0, m_lineWidth / 2.0f, 0}, ngl::Vec3{0, m_lineWidth / 2.0f, 0.9f}, ngl::Vec3{0, m_lineWidth, 0.9f}, ngl::Vec3{0, 0, 1},
                ngl::Vec3{0, -m_lineWidth, 0.9f}, ngl::Vec3{0, -m_lineWidth / 2.0f, 0.9f}, ngl::Vec3{0, -m_lineWidth / 2.0f, 0}};
    triangleVerts = 
                    { linePoints[0], linePoints[6], linePoints[1],
                    linePoints[1], linePoints[3], linePoints[2],
                    linePoints[3], linePoints[1], linePoints[5],
                    linePoints[4], linePoints[3], linePoints[5],
                    linePoints[6], linePoints[5], linePoints[1]};
    m_translateMeshes[2]->SetVertices(triangleVerts);

    // Rotation Gizmos
    linePoints.clear();
    triangleVerts.clear();
    const float segmentAngle = ngl::radians( 90.0f / m_rotateGizmoSegments );

    linePoints.push_back(ngl::Vec3{ cos(0) * (m_gizmoRadius - m_lineWidth), sin(0) * (m_gizmoRadius - m_lineWidth), 0});
    linePoints.push_back(ngl::Vec3{ cos(0) * m_gizmoRadius, sin(0) * m_gizmoRadius, 0});
    int vertIndex = 1;
    for(int i = 1; i <= m_rotateGizmoSegments; ++i)
    {
        linePoints.push_back(ngl::Vec3{ cos(segmentAngle * i) * (m_gizmoRadius - m_lineWidth), sin(segmentAngle * i) * (m_gizmoRadius - m_lineWidth), 0});
        linePoints.push_back(ngl::Vec3{ cos(segmentAngle * i) * m_gizmoRadius, sin(segmentAngle * i) * m_gizmoRadius, 0});
        vertIndex += 2;
        triangleVerts.insert(triangleVerts.end(), {linePoints[vertIndex - 3], linePoints[vertIndex - 2], linePoints[vertIndex - 1]});
        triangleVerts.insert(triangleVerts.end(), {linePoints[vertIndex - 2], linePoints[vertIndex], linePoints[vertIndex - 1]});
    }
    m_rotateMeshes[2]->SetVertices(triangleVerts);
    linePoints.clear();
    triangleVerts.clear();

    linePoints.push_back(ngl::Vec3{ cos(0) * (m_gizmoRadius - m_lineWidth), 0, sin(0) * (m_gizmoRadius - m_lineWidth)});
    linePoints.push_back(ngl::Vec3{ cos(0) * m_gizmoRadius, 0, sin(0) * m_gizmoRadius});
    vertIndex = 1;
    for(int i = 1; i <= m_rotateGizmoSegments; ++i)
    {
        linePoints.push_back(ngl::Vec3{ cos(segmentAngle * i) * (m_gizmoRadius - m_lineWidth), 0, sin(segmentAngle * i) * (m_gizmoRadius - m_lineWidth)});
        linePoints.push_back(ngl::Vec3{ cos(segmentAngle * i) * m_gizmoRadius, 0, sin(segmentAngle * i) * m_gizmoRadius});
        vertIndex += 2;
        triangleVerts.insert(triangleVerts.end(), {linePoints[vertIndex - 3], linePoints[vertIndex - 1], linePoints[vertIndex - 2]});
        triangleVerts.insert(triangleVerts.end(), {linePoints[vertIndex - 1], linePoints[vertIndex], linePoints[vertIndex - 2]});
    }
    m_rotateMeshes[1]->SetVertices(triangleVerts);

    linePoints.clear();
    triangleVerts.clear();
    linePoints.push_back(ngl::Vec3{0, cos(0) * (m_gizmoRadius - m_lineWidth), sin(0) * (m_gizmoRadius - m_lineWidth)});
    linePoints.push_back(ngl::Vec3{0, cos(0) * m_gizmoRadius, sin(0) * m_gizmoRadius});
    vertIndex = 1;
    for(int i = 1; i <= m_rotateGizmoSegments; ++i)
    {
        linePoints.push_back(ngl::Vec3{0, cos(segmentAngle * i) * (m_gizmoRadius - m_lineWidth), sin(segmentAngle * i) * (m_gizmoRadius - m_lineWidth)});
        linePoints.push_back(ngl::Vec3{0, cos(segmentAngle * i) * m_gizmoRadius, sin(segmentAngle * i) * m_gizmoRadius});
        vertIndex += 2;
        triangleVerts.insert(triangleVerts.end(), {linePoints[vertIndex - 3], linePoints[vertIndex - 1], linePoints[vertIndex - 2]});
        triangleVerts.insert(triangleVerts.end(), {linePoints[vertIndex - 1], linePoints[vertIndex], linePoints[vertIndex - 2]});
    }
    m_rotateMeshes[0]->SetVertices(triangleVerts);

    // Scale Gizmos
    linePoints =
                {ngl::Vec3{0, m_lineWidth / 2.0f, 0}, ngl::Vec3{0.9f, m_lineWidth / 2.0f, 0}, ngl::Vec3{0.9f, m_lineWidth, 0}, ngl::Vec3{1, m_lineWidth, 0},
                ngl::Vec3{1, -m_lineWidth, 0}, ngl::Vec3{0.9f, -m_lineWidth, 0}, ngl::Vec3{0.9f, -m_lineWidth / 2.0f, 0}, ngl::Vec3{0.0f, -m_lineWidth / 2.0f, 0} };

    triangleVerts = 
                    { linePoints[0], linePoints[7], linePoints[1],
                    linePoints[1], linePoints[3], linePoints[2],
                    linePoints[3], linePoints[1], linePoints[6],
                    linePoints[4], linePoints[3], linePoints[6],
                    linePoints[6], linePoints[5], linePoints[4],
                    linePoints[7], linePoints[6], linePoints[1]};
    m_scaleMeshes[0]->SetVertices(triangleVerts);

    linePoints =
                {ngl::Vec3{m_lineWidth / 2.0f, 0, 0}, ngl::Vec3{m_lineWidth / 2.0f, 0.9f, 0}, ngl::Vec3{m_lineWidth, 0.9f, 0}, ngl::Vec3{m_lineWidth, 1, 0},
                ngl::Vec3{-m_lineWidth, 1, 0}, ngl::Vec3{-m_lineWidth, 0.9f, 0}, ngl::Vec3{-m_lineWidth / 2.0f, 0.9f, 0}, ngl::Vec3{-m_lineWidth / 2.0f, 0, 0} };
    triangleVerts = 
                    { linePoints[0], linePoints[7], linePoints[1],
                    linePoints[1], linePoints[3], linePoints[2],
                    linePoints[3], linePoints[1], linePoints[6],
                    linePoints[4], linePoints[3], linePoints[6],
                    linePoints[6], linePoints[5], linePoints[4],
                    linePoints[7], linePoints[6], linePoints[1]};
    m_scaleMeshes[1]->SetVertices(triangleVerts);

    linePoints =
                {ngl::Vec3{0, m_lineWidth / 2.0f, 0}, ngl::Vec3{0, m_lineWidth / 2.0f, 0.9f}, ngl::Vec3{0, m_lineWidth, 0.9f}, ngl::Vec3{0, m_lineWidth, 1},
                ngl::Vec3{0, -m_lineWidth, 1}, ngl::Vec3{0, -m_lineWidth, 0.9f}, ngl::Vec3{0, -m_lineWidth / 2.0f, 0.9f}, ngl::Vec3{0, -m_lineWidth / 2.0f, 0} };
    triangleVerts = 
                    { linePoints[0], linePoints[7], linePoints[1],
                    linePoints[1], linePoints[3], linePoints[2],
                    linePoints[3], linePoints[1], linePoints[6],
                    linePoints[4], linePoints[3], linePoints[6],
                    linePoints[6], linePoints[5], linePoints[4],
                    linePoints[7], linePoints[6], linePoints[1]};
    m_scaleMeshes[2]->SetVertices(triangleVerts);

    m_colors[0] = {1, 0, 0, 1};
    m_colors[1] = {0, 1, 0, 1};
    m_colors[2] = {0, 0, 1, 1};

    ngl::ShaderLib::loadShader("UnlitConstantSize", "shaders/UnlitVert.glsl", "shaders/UnlitFrag.glsl");
    //ngl::ShaderLib::use(m_name);
}

void Gizmo::Draw(const ngl::Mat4& vp, int windowWidth)
{
    //m_transform.setScale(ngl::Vec3{1.0f,1.0f,1.0f});

    std::unique_ptr<Mesh>* meshesToDraw = nullptr;

    switch (m_type){
            case GizmoType::TRANSLATE:
                    meshesToDraw = m_translateMeshes.data();
                    break;
            case GizmoType::ROTATE:
                    meshesToDraw = m_rotateMeshes.data();
                    break;
            case GizmoType::SCALE:
                    meshesToDraw = m_scaleMeshes.data();
                    break;
            default: break;
    }
    if(meshesToDraw != nullptr)
    {
        ngl::ShaderLib::use("UnlitConstantSize");
        glDisable(GL_DEPTH_TEST);
        ngl::ShaderLib::setUniform("MVP", vp * m_transform.getMatrix());
        ngl::ShaderLib::setUniform("reciprScaleOnScreen", 2.0f * m_sizeOnScreen / windowWidth);
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
            meshesToDraw[i]->DrawWireframe();
                //glLineWidth(6);
                //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE_STRIP);
                //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                //glLineWidth(1);
            
        }
        glEnable(GL_DEPTH_TEST);
    }

        
}

void Gizmo::Manipulate(ngl::Transformation& objectTransform, float dx, float dy)
{
    ngl::Vec3 moveDirection;
    float axis;
    switch (m_type){
        case GizmoType::TRANSLATE:
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
        case GizmoType::SCALE:
            switch (m_selectedAxis)
            {
                case 0:
                    moveDirection = ngl::Vec3(1, 0, 0);
                    axis = dx;
                    break;
                case 1:
                    moveDirection = ngl::Vec3(0, 1, 0);
                    axis = -dy;
                    break;
                case 2:
                    moveDirection = ngl::Vec3(0, 0, 1);
                    axis = dx;
                    break;
            }

            objectTransform.addScale( moveDirection * axis * 0.005f );
            break;
        case GizmoType::ROTATE:
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
            objectTransform.addRotation( - moveDirection * axis * 0.5f);
            break;
    }
}