#ifndef NGLSCENE_H_
#define NGLSCENE_H_
#include <ngl/Vec3.h>
#include <ngl/Transformation.h>
#include <ngl/Mat4.h>
#include <memory>
#include "Camera.h"
#include "WindowParams.h"
#include "PBRShaderManager.h"
#include "ObjMesh.h"
#include "MeshObject.h"
#include "FrameBuffer.h"
#include "Gizmos.h"
// this must be included after NGL includes else we get a clash with gl libs
//#include <QOpenGLWindow>
#include <QGLWidget>
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

class NGLScene : public QGLWidget
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

    signals:
        void UpdateTransformUI(ngl::Transformation);
        void UpdateSceneListUI(std::vector<std::shared_ptr<SceneObject>>);
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
    /// @brief windows parameters for mouse control etc.
    WinParams m_win;

    //ngl::Vec3 m_modelPos = {0.0f, 0.0f, 0.0f};
    Camera m_camera;
    std::unique_ptr<Gizmo> m_gizmo;

    std::vector<Light> m_directionalLights;
    std::vector<Light> m_pointLights;
    //std::unique_ptr<PBRShaderManager> m_shaderManager;
    std::vector< std::shared_ptr<SceneObject> > m_sceneObjects;

    std::shared_ptr<SceneObject> m_selectedObject;

    std::unique_ptr<FrameBuffer> m_viewportFrameBuffer;
};



#endif
