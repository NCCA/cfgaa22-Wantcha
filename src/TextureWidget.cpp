#include "TextureWidget.h"
#include <QGLFormat>
#include <QFileDialog>
#include <iostream>
//#include <ngl/Texture.h>

TextureWidget::TextureWidget(GLuint* textureID, int width, int height, std::shared_ptr<ngl::Texture> texture)
    :m_id(textureID), m_texture(texture)
{
    m_target.setWidth(width);
    m_target.setHeight(height);
}

TextureWidget::~TextureWidget()
{
  //std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
  //glDeleteTextures(1, &m_id);
}

void TextureWidget::initializeGL()
{
    /*QSurfaceFormat format;
    format.setSamples(4);
    #if defined( __APPLE__)
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    #else
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    #endif
    format.setProfile(QSurfaceFormat::CoreProfile);
    this->setFormat(format);*/

    if( ngl::ShaderLib::getProgramID("SimpleTexture") == 0)
    {
        ngl::ShaderLib::loadShader("SimpleTexture", "shaders/SimpleTextureVert.glsl", "shaders/SimpleTextureFrag.glsl");
    }

    ngl::Vec3 points[4] = { ngl::Vec3{ -1, -1, 0 }, ngl::Vec3{ 1, -1, 0 }, ngl::Vec3{ 1, 1, 0 }, ngl::Vec3{ -1, 1, 0 } };
    ngl::Vec2 uvs[] = { ngl::Vec2{ 0,0 }, ngl::Vec2{ 1, 0 }, ngl::Vec2{ 1,1 }, ngl::Vec2{ 0,1 } };

    m_vbo[0].pos = points[0]; m_vbo[0].uv = uvs[0];
    m_vbo[1].pos = points[1]; m_vbo[1].uv = uvs[1];
    m_vbo[2].pos = points[2]; m_vbo[2].uv = uvs[2];
    m_vbo[3].pos = points[3]; m_vbo[3].uv = uvs[3];
    m_vbo[4].pos = points[0]; m_vbo[4].uv = uvs[0];
    m_vbo[5].pos = points[2]; m_vbo[5].uv = uvs[2];

    m_vaoMesh = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_TRIANGLES);
    m_vaoMesh->bind();

    m_vaoMesh->setData(ngl::SimpleVAO::VertexData(6*sizeof(SimpleVertData), m_vbo[0].pos.m_x));

    m_vaoMesh->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(SimpleVertData), 0);

    m_vaoMesh->setVertexAttributePointer(1, 2, GL_FLOAT, sizeof(SimpleVertData), 3);

    m_vaoMesh->setNumIndices(6);

    m_vaoMesh->unbind();
}

void TextureWidget::resizeGL(int width, int height)
{
    m_target.setWidth(width);
    m_target.setHeight(height);
}

void TextureWidget::paintGL()
{
    makeCurrent();
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    //glDisable(GL_CULL_FACE);
    //glPointSize(2);

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, m_target.size().width(), m_target.size().height());

    ngl::ShaderLib::use("SimpleTexture");
    ngl::ShaderLib::setUniform("colorTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *m_id);

    m_vaoMesh->bind();
    m_vaoMesh->draw();
    m_vaoMesh->unbind();
    //std::cout<<"ID "<< m_id <<"\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    doneCurrent();
}

void TextureWidget::mousePressEvent(QMouseEvent *event)
{
    QString q_filepath = QFileDialog::getOpenFileName(this, "Select texture file");
    /*if(!q_filepath.endsWith(".obj", Qt::CaseSensitivity::CaseInsensitive))
    {
        QMessageBox::information(this, "Error!", "The selected file is not an .obj!");
        return;
    }*/
    std::string filepath = q_filepath.toStdString();
    m_texture = std::make_shared<ngl::Texture>(filepath);
    m_texture->setMultiTexture(0);
    *m_id = m_texture->setTextureGL();

}

void TextureWidget::mouseMoveEvent(QMouseEvent *event)
{}

void TextureWidget::mouseReleaseEvent(QMouseEvent *event)
{}

QSize TextureWidget::minimumSizeHint() const
{
    return QSize(20, 20);
}

QSize TextureWidget::sizeHint() const
{
    return QSize(m_target.size().width(), m_target.size().height());
}
