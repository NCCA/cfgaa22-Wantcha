#include "TextureWidget.h"
#include <QGLFormat>
#include <iostream>
//#include <ngl/Texture.h>

QString vertexShaderSource =
"#version 330 core\n\
layout (location = 0) in vec3 aPos;\n\
void main()\n\
{\n\
gl_Position = vec4(aPos, 1.0);\n\
}";
QString fragmentShaderSource =
"#version 330 core\n\
out vec4 FragColor;\n\
void main()\n\
{\n\
FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n\
}";

float vertices[] = {
    0.5f, 0.5f, 0.0f,   
    0.5f, -0.5f, 0.0f,    
   -0.5f, -0.5f, 0.0f,
   -0.5f,  0.5f, 0.0f
   };

unsigned int indices[] =
{
    0, 1, 3,
    1, 2, 3
};

TextureWidget::TextureWidget(int textureID, int width, int height)
    :m_id(textureID)
{
    m_target.setWidth(width);
    m_target.setHeight(height);


    ngl::Vec3 points[] = { ngl::Vec3{ -10000, -10000, -1 }, ngl::Vec3{ 10000, -10000, -1 }, ngl::Vec3{ 10000, 10000, -1 }, ngl::Vec3{ -10000, 10000, -1 } };
    ngl::Vec2 uvs[] = { ngl::Vec2{ 0,0 }, ngl::Vec2{ 1, 0 }, ngl::Vec2{ 1,1 }, ngl::Vec2{ -0,1 } };

    std::vector<ngl::Vec3> verts = { points[0], points[1], points[2], points[3], points[0], points[2] };
    m_vbo[0].pos = points[0];// m_vbo[0].uv = uvs[0];
    m_vbo[1].pos = points[1];// m_vbo[1].uv = uvs[1];
    m_vbo[2].pos = points[2];// m_vbo[2].uv = uvs[2];
    m_vbo[3].pos = points[3];// m_vbo[3].uv = uvs[3];
    m_vbo[4].pos = points[0];// m_vbo[4].uv = uvs[0];
    m_vbo[5].pos = points[2];// m_vbo[5].uv = uvs[2];

    m_vaoMesh = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_POINTS);
    m_vaoMesh->bind();

    m_vaoMesh->setData(ngl::SimpleVAO::VertexData(6*sizeof(SimpleVertData), m_vbo[0].pos.m_x));

    m_vaoMesh->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(SimpleVertData), 0);

    //m_vaoMesh->setVertexAttributePointer(1, 2, GL_FLOAT, sizeof(SimpleVertData), 3);

    m_vaoMesh->setNumIndices(6);

    m_vaoMesh->unbind();

    if( ngl::ShaderLib::getProgramID("SimpleTexture") == 0)
    {
        ngl::ShaderLib::loadShader("SimpleTexture", "shaders/SimpleTextureVert.glsl", "shaders/SimpleTextureFrag.glsl");
    }
}

TextureWidget::~TextureWidget()
{
  //std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void TextureWidget::initializeGL()
{
    // enable multisampling for smoother drawing
    //glEnable(GL_MULTISAMPLE);
    /*initializeGLFunctions();
    vertexBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
    indexBuffer = new QGLBuffer(QGLBuffer::IndexBuffer);

    shader.addShaderFromSourceCode(QGLShader::Vertex, vertexShaderSource);
    shader.addShaderFromSourceCode(QGLShader::Fragment, fragmentShaderSource);
    shader.link();

    shader.bind();
    vertexBuffer->create();
    vertexBuffer->bind();
    vertexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
    vertexBuffer->allocate(vertices, sizeof(vertices));
    shader.setAttributeBuffer("aPos", GL_FLOAT, 0, 3, 12);
    vertexBuffer->release();
    shader.release();

    indexBuffer->create();
    indexBuffer->bind();
    indexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
    indexBuffer->allocate(indices, sizeof(indices));
    indexBuffer->release();*/

}

void TextureWidget::resizeGL(int width, int height)
{
    m_target.setWidth(width);
    m_target.setHeight(height);
}

void TextureWidget::paintGL()
{
    makeCurrent();
    //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 1, 1);

    ngl::ShaderLib::use("SimpleTexture");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_id);

    m_vaoMesh->bind();
    m_vaoMesh->draw();
    m_vaoMesh->unbind();

    /*resize(m_target.size().width(), m_target.size().height());
    glViewport(0, 0, m_target.size().width(), m_target.size().height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    indexBuffer->bind();
    shader.bind();
    shader.enableAttributeArray("aPos");
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    shader.disableAttributeArray("aPos");
    shader.release();
    indexBuffer->release();*/
}

void TextureWidget::mousePressEvent(QMouseEvent *event)
{}

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
