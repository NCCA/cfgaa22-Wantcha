#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Shader.h>
#include <iostream>
#include <fstream>
//#include <CGAL/Bbox_3.h>

NGLScene::NGLScene()
{
  setFocusPolicy(Qt::FocusPolicy::StrongFocus);

  m_camera = Camera(45.0f, 1600.0f / 900.0f, 0.01f, 1000.0f);
  QGLFormat format;
  format.setSamples(4);
  #if defined( __APPLE__)
    // at present mac osx Mountain Lion only supports GL3.2
    // the new mavericks will have GL 4.x so can change
    format.toSurfaceFormat(format).setMajorVersion(4);
    format.toSurfaceFormat(format).setMajorVersion(1);
  #else
    // with luck we have the latest GL version so set to this
    format.toSurfaceFormat(format).setMajorVersion(4);
    format.toSurfaceFormat(format).setMajorVersion(3);
  #endif
  setFormat(format);

  m_shaderManager = std::make_unique<PBRShaderManager>("Basic", "shaders/BasicVert.glsl", "shaders/BasicFrag.glsl");
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );

  m_camera.SetViewportSize(_w, _h);
}


void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  ngl::NGLInit::initialize();
  glClearColor(0.15f, 0.15f, 0.18f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  DirectionalLight l1({-0.98f, 0.0f, 0.1f}, {1.0f, 1.0f, 1.0f}, 0.6f);
  DirectionalLight l2({0.7f, 0.7f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.9f);
  //DirectionalLight l3({0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f});

  m_directionalLights.push_back(l1);
  m_directionalLights.push_back(l2);

  PointLight pl1(ngl::Vec3(0.5f, 0.0f, -0.5f));
  m_pointLights.push_back(pl1);

  m_shaderManager->UpdateLightCounts(m_directionalLights, m_pointLights);

  //ngl::ShaderLib::setUniform("directionalLightCount", static_cast<int>(m_directionalLights.size()));

  m_mesh = std::make_unique<ObjMesh>("meshes/yuri.obj");
}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);

  ngl::Mat4 MV = m_camera.GetView() * m_mesh->GetTransform().getMatrix();
  ngl::Mat3 normalMatrix = MV.inverse().transpose();
  ngl::Mat4 MVP = m_camera.GetProjection() * MV;

  m_shaderManager->UseShader();
  ngl::ShaderLib::setUniform("MVP", MVP);
  //ngl::ShaderLib::setUniform("NormalMatrix", normalMatrix);

  //ngl::VAOPrimitives::draw(ngl::troll);
  m_mesh->Draw();

  ngl::ShaderLib::use(ngl::nglColourShader);
  ngl::ShaderLib::setUniform("MVP", MVP);
  ngl::ShaderLib::setUniform("Colour", ngl::Vec4(1.0f, 0.7f, 0.05f, 1.0f));
  //glPointSize(2);
  glLineWidth(1);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  m_mesh->Draw();
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  //std::cout<<"NUUU";
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_A :
      //m_modelPos.set(ngl::Vec3::zero());
      //m_directionalLights.push_back(DirectionalLight({0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}));
      m_directionalLights.push_back(DirectionalLight({0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, 2.0f));
      m_shaderManager->UpdateLightCounts(m_directionalLights, m_pointLights);
      //std::cout<<"DAAAAAA\n";

  break;
  default : break;
  }
  // finally update the GLWindow and re-draw

  update();
}

/*void NGLScene::keyReleaseEvent(QKeyEvent *_event)
{
  switch (_event->key())
  {
  break;
  default : break;
  }
  // finally update the GLWindow and re-draw

    //update();
}*/
