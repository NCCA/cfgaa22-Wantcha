#include "TextureWidget.h"
#include <QGLFormat>
#include <QFileDialog>
#include <iostream>
//#include <ngl/Texture.h>

TextureWidget::TextureWidget(GLuint* textureID, int width, int height, std::shared_ptr<ngl::Texture>* texture)
    :m_id(textureID), m_texture(texture)
{
    m_target.setWidth(width);
    m_target.setHeight(height);
    m_modifyTexture = true;
}

TextureWidget::TextureWidget(GLuint* textureID, int width, int height)
    :m_id(textureID)
{
    m_target.setWidth(width);
    m_target.setHeight(height);
    m_modifyTexture = false;
}

TextureWidget::~TextureWidget()
{
}

void TextureWidget::initializeGL()
{

}

void TextureWidget::resizeGL(int width, int height)
{
    m_target.setWidth(width);
    m_target.setHeight(height);
}

void TextureWidget::paintGL()
{
    if(m_initialized)
    {
        glDisable(GL_CULL_FACE);  
        //makeCurrent();
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_CULL_FACE); 
        //doneCurrent();
    }
}

void TextureWidget::SetTexture(GLuint* textureID, std::shared_ptr<ngl::Texture>* texture, bool modifyTexture)
{
    makeCurrent();
    if(!m_initialized)
    {
        struct SimpleVertData
        {
            ngl::Vec3 pos;
            ngl::Vec2 uv;
        };

        ngl::Vec3 points[4] = { ngl::Vec3{ -1, -1, 0 }, ngl::Vec3{ 1, -1, 0 }, ngl::Vec3{ 1, 1, 0 }, ngl::Vec3{ -1, 1, 0 } };
        ngl::Vec2 uvs[4] = { ngl::Vec2{ 0,0 }, ngl::Vec2{ 1, 0 }, ngl::Vec2{ 1,1 }, ngl::Vec2{ 0,1 } };

        SimpleVertData vbo[6];
        vbo[0].pos = points[0]; vbo[0].uv = uvs[0];
        vbo[1].pos = points[1]; vbo[1].uv = uvs[1];
        vbo[2].pos = points[2]; vbo[2].uv = uvs[2];
        vbo[3].pos = points[3]; vbo[3].uv = uvs[3];
        vbo[4].pos = points[0]; vbo[4].uv = uvs[0];
        vbo[5].pos = points[2]; vbo[5].uv = uvs[2];

        m_vaoMesh = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_TRIANGLES);
        m_vaoMesh->bind();

        m_vaoMesh->setData(ngl::SimpleVAO::VertexData(6*sizeof(SimpleVertData), vbo[0].pos.m_x));
        m_vaoMesh->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(SimpleVertData), 0);
        m_vaoMesh->setVertexAttributePointer(1, 2, GL_FLOAT, sizeof(SimpleVertData), 3);
        m_vaoMesh->setNumIndices(6);

        m_vaoMesh->unbind();
        m_initialized = true;
        //std::cout<<"INITIALIZED\n";
    }
    if(modifyTexture == false)
    {
        m_modifyTexture = false;
    }
    else
    {
        m_texture = texture;
        m_modifyTexture = true;
    }
    m_id = textureID;
    update();
}

void TextureWidget::mousePressEvent(QMouseEvent *event)
{
    QString q_filepath = QFileDialog::getOpenFileName(this, "Select texture file");
    std::string filepath = q_filepath.toStdString();
    if(m_modifyTexture)
    {
        *m_texture = AssetManager::GetAsset<ngl::Texture>(filepath);
        m_texture->get()->setMultiTexture(0);
        *m_id = m_texture->get()->setTextureGL();
        //std::cout<<m_texture.get()<<"\n";
    }
    emit selectedPath(filepath);
    update();
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
