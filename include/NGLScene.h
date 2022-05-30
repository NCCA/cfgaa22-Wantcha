#ifndef NGLSCENE_H_
#define NGLSCENE_H_
#include <ngl/Vec3.h>
#include "Transform.h"
#include <ngl/Mat4.h>
#include <memory>
#include "Camera.h"
#include "WindowParams.h"
#include "PBRShaderManager.h"
#include "ObjMesh.h"
#include "MeshObject.h"
#include "FrameBuffer.h"
#include "Gizmos.h"
#include "EnvironmentTexture.h"
// this must be included after NGL includes else we get a clash with gl libs
//#include <QOpenGLWindow>
#include <QOpenGLWidget>
//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QOpenGLWidget
{
  Q_OBJECT
  public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for our NGL drawing class
    /// @param [in] parent the parent window to the class
    //----------------------------------------------------------------------------------------------------------------------
    NGLScene();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor must close down ngl and release OpenGL resources
    //----------------------------------------------------------------------------------------------------------------------
    ~NGLScene() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the initialize class is called once when the window is created and we have a valid GL context
    /// use this to setup any default GL stuff
    //----------------------------------------------------------------------------------------------------------------------
    void initializeGL() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we want to draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void paintGL() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we resize the window
    //----------------------------------------------------------------------------------------------------------------------
    void resizeGL(int _w, int _h) override;

    float getAmbientIntensity() const { return m_ambientIntensity; }
    void setAmbientIntensity(float intensity) { m_ambientIntensity = intensity; }
    EnvironmentTexture* getEnvironmentMap() { return m_environment.get(); }
    std::vector< std::shared_ptr<SceneObject> >& getSceneObjects() { return m_sceneObjects; }
    bool getRenderEnvironment() const { return m_renderEnvironment; }
    void setRenderEnvironment(bool render) { m_renderEnvironment = render; }
    void resetScene();
    void DeselectAll() { m_selectedObject = nullptr; }
    std::shared_ptr<SceneObject> GetSelectedObject() { return m_selectedObject; }
    void SetBackgroundColor(const ngl::Vec3& col) { m_backgroundColor = col; }
    ngl::Vec3 GetBackgroundColor() const { return m_backgroundColor; }

    public slots:
      void setPosX(double val);
      void setPosY(double val);
      void setPosZ(double val);

      void setRotX(double val);
      void setRotY(double val);
      void setRotZ(double val);

      void setScaleX(double val);
      void setScaleY(double val);
      void setScaleZ(double val);

      void OnSceneListItemSelected(int index);
      void OnSceneListItemDeleted(int index);
      void OnAddMesh(const std::string& path);
      void OnAddDirectionalLight();
      void OnAddPointLight();
      void OnSaveFramebuffer(const std::string& filepath);
      void OnSaveFramebufferSize(const std::string& filepath, int width, int height);

    signals:
        void UpdateTransformUI(Transform);
        void UpdateSceneListUI(const std::vector<std::shared_ptr<SceneObject>>&);
        void UpdatePropertiesBox(std::shared_ptr<SceneObject>);
        void UpdateSelectedIndex(int index);
private:

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Qt Event called when a key is pressed
    /// @param [in] _event the Qt event to query for size etc
    //----------------------------------------------------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent *_event) override;
    //void keyReleaseEvent(QKeyEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse is moved
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent (QMouseEvent * _event ) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is pressed
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent ( QMouseEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is released
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent ( QMouseEvent *_event ) override;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse wheel is moved
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent( QWheelEvent *_event) override;

    void RenderFramebuffer(int width, int height);
    /// @brief windows parameters for mouse control etc.
    WinParams m_win;

    //ngl::Vec3 m_modelPos = {0.0f, 0.0f, 0.0f};
    std::shared_ptr<Camera> m_camera;
    std::unique_ptr<Gizmo> m_gizmo;
    //std::unique_ptr<PBRShaderManager> m_shaderManager;
    std::vector< std::shared_ptr<SceneObject> > m_sceneObjects;

    std::shared_ptr<SceneObject> m_selectedObject;

    std::unique_ptr<FrameBuffer> m_viewportFrameBuffer;

    std::unique_ptr<EnvironmentTexture> m_environment;
    bool m_renderEnvironment = false;

    float m_ambientIntensity = 1.0f;

    int m_hoveredObjectID = -100;
    ngl::Vec3 m_backgroundColor = ngl::Vec3(0.14f, 0.14f, 0.18f);
};



#endif
