#ifndef TEXTURE_WIDGET_H
#define TEXTURE_WIDGET_H

#include <ngl/AbstractVAO.h>
#include <ngl/SimpleVAO.h>
#include <ngl/Vec3.h>
#include <ngl/Vec2.h>
//#include <SceneObject.h>
#include <string>
#include <Mesh.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOFactory.h>
#include <QGLWidget>
#include <QOpenGLWidget>
/*#include <QGLShader>
#include <QGLBuffer>
#include <QGLFunctions>
#include <memory>*/


class TextureWidget : public QOpenGLWidget//, protected QGLFunctions
{
Q_OBJECT

public:
    TextureWidget(GLuint* textureID, int width, int height, std::shared_ptr<ngl::Texture> texture);
    TextureWidget(GLuint* textureID, int width, int height);

    ~TextureWidget() override;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    signals:
    void selectedPath(const std::string& path);

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    GLuint* m_id = 0;
    bool m_modifyTexture = false;
    std::unique_ptr<ngl::AbstractVAO> m_vaoMesh;

    QRectF m_target;
    std::shared_ptr<ngl::Texture> m_texture;
};

#endif