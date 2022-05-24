#ifndef TEXTURE_WIDGET_H
#define TEXTURE_WIDGET_H

#include <ngl/AbstractVAO.h>
#include <ngl/SimpleVAO.h>
#include <ngl/Vec3.h>
#include <ngl/Vec2.h>
#include <Mesh.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOFactory.h>
#include <QGLWidget>
/*#include <QGLShader>
#include <QGLBuffer>
#include <QGLFunctions>
#include <memory>*/

struct SimpleVertData
{
    ngl::Vec3 pos;
    //ngl::Vec2 uv;
};

class TextureWidget : public QGLWidget//, protected QGLFunctions
{
Q_OBJECT

public:
    TextureWidget(int textureID, int width, int height);

    ~TextureWidget() override;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
//signals:
    //void clicked();

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    int m_id = 0;
    SimpleVertData m_vbo[6];
    std::unique_ptr<ngl::AbstractVAO> m_vaoMesh;

    QRectF m_target;

    /*QGLShaderProgram shader;
    QGLBuffer* vertexBuffer;
    QGLBuffer* indexBuffer;
    float viewPortWidth;
    float viewPortHeight;*/

};

#endif