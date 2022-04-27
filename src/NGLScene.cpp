#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Shader.h>
#include <iostream>

#include "MeshObject.h"
#include "Assets/AssetManager.h"

NGLScene::NGLScene()
{
  setFocusPolicy(Qt::FocusPolicy::StrongFocus);
  setMouseTracking(true);

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

  AssetManager::RegisterCache(std::make_unique<AssetCache<ObjMesh>>());
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
  m_viewportFrameBuffer->Resize(_w, _h);
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

  FramebufferSpecification fbSpec;
	fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
	fbSpec.Width = 1600;
	fbSpec.Height = 900;
	m_viewportFrameBuffer = std::make_unique<FrameBuffer>(fbSpec);


  Light l1(LightType::Directional,
        {0, 0, 0}, { 180, 0 ,0 }, {1.0f, 1.0f, 1.0f}, 0.6f);

  Light l2(LightType::Directional, {0, 0, 0}, { 0, 0, 0 },
            {1.0f, 1.0f, 1.0f}, 0.9f);

  m_directionalLights.push_back(l1);
  //m_directionalLights.push_back(l2);

  Light pl1(LightType::Point, ngl::Vec3(0.5f, 0.0f, -0.5f), ngl::Vec3(90, 0, 0));
  m_pointLights.push_back(pl1);

  PBRShaderManager::Init("Basic", "shaders/BasicVert.glsl", "shaders/BasicFrag.glsl");
  PBRShaderManager::UpdateLightCounts(m_directionalLights, m_pointLights);

  m_gizmo = std::make_unique<Gizmo>();
  //ngl::ShaderLib::setUniform("directionalLightCount", static_cast<int>(m_directionalLights.size()));


  m_sceneObjects.push_back(std::make_shared<MeshObject>("meshes/arrow.obj"));
  //static_cast<MeshObject>(m_sceneObjects[0])->GetMesh()->GetMaterial().SetTexture("textures/checkerboard.jpg");
  m_sceneObjects[0]->SetPosition({0.1f, 0.23f, 0.05f});
  m_selectedObject = m_sceneObjects[0];

  m_sceneObjects.push_back(std::make_shared<MeshObject>("meshes/yuri.obj"));
  m_sceneObjects[1]->SetPosition({0.5f, -0.23f, -0.5f});
  m_sceneObjects[1]->SetScale({0.5f, 0.5f, 0.5f});
  m_sceneObjects[1]->SetName("Buncf");

  emit UpdateSceneListUI(m_sceneObjects);
  emit UpdateTransformUI(m_selectedObject->GetTransform());
}



void NGLScene::paintGL()
{
  m_viewportFrameBuffer->Bind();
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_viewportFrameBuffer->ClearAttachment(1, -100);

  int it = 0;

  if(m_selectedObject)
  {
    ngl::Transformation trans = m_selectedObject->GetTransform();
    trans.setScale(ngl::Vec3{1.0f,1.0f,1.0f});
    trans.getMatrix(); /*recomputing matrix*/
    m_gizmo->SetTransform(trans);
  }
  
  ngl::Mat4 VP = m_camera.GetProjection() * m_camera.GetView();

  for(auto mesh : m_sceneObjects)
  {
    PBRShaderManager::UseShader();
    //ngl::Mat3 normalMatrix = MV.inverse().transpose();
    ngl::Mat4 MVP = VP * mesh->GetTransform().getMatrix();
    //ngl::ShaderLib::setUniform("NormalMatrix", normalMatrix);

    ngl::ShaderLib::setUniform("MVP", MVP);
    ngl::ShaderLib::setUniform("objectID", it);

    mesh->Draw();
    if(mesh == m_selectedObject)
    {
      ngl::ShaderLib::use(ngl::nglColourShader);
      ngl::ShaderLib::setUniform("MVP", MVP);
      ngl::ShaderLib::setUniform("Colour", ngl::Vec4(1.0f, 0.7f, 0.05f, 1.0f));
      //glPointSize(2);
      //glLineWidth(1);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      mesh->DrawHighlighted();
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    ++it;
  }
  m_gizmo->Draw(VP, m_win.width);

  /*ngl::ShaderLib::use(ngl::nglColourShader);
  ngl::ShaderLib::setUniform("MVP", MVP);
  ngl::ShaderLib::setUniform("Colour", ngl::Vec4(1.0f, 0.7f, 0.05f, 1.0f));
  //glPointSize(2);
  glLineWidth(1);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  m_mesh->Draw();
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);*/

  m_viewportFrameBuffer->BlitToScreen();

  m_viewportFrameBuffer->Unbind();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_A :
      //m_directionalLights.push_back(DirectionalLight({0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}));
      m_directionalLights.push_back(
        Light(LightType::Directional, {0.0f, 0.0f, 0.0f}, {90, 90, 0}, {1.0f, 1.0f, 0.0f}, 2.0f) );

      PBRShaderManager::UpdateLightCounts(m_directionalLights, m_pointLights);

  break;
  default : break;
  }
  // finally update the GLWindow and re-draw

  update();
}

void NGLScene::setPosX(double val)
{
  if(m_selectedObject)
  {
    ngl::Vec3 pos = m_selectedObject->GetTransform().getPosition();
    m_selectedObject->GetTransform().setPosition
                    (ngl::Vec3{static_cast<ngl::Real>(val), pos.m_y, pos.m_z});
    update();
  }
}

void NGLScene::setPosY(double val)
{
  if(m_selectedObject)
  {
    ngl::Vec3 pos = m_selectedObject->GetTransform().getPosition();
    m_selectedObject->GetTransform().setPosition
                    (ngl::Vec3{pos.m_x, static_cast<ngl::Real>(val), pos.m_z});
    update();
  }
}

void NGLScene::setPosZ(double val)
{
  if(m_selectedObject)
  {
    ngl::Vec3 pos = m_selectedObject->GetTransform().getPosition();
    m_selectedObject->GetTransform().setPosition
                    (ngl::Vec3{ pos.m_x, pos.m_y, static_cast<ngl::Real>(val) });
    update();
  }
}

void NGLScene::setRotX(double val)
{
  if(m_selectedObject)
  {
    ngl::Vec3 rot = m_selectedObject->GetTransform().getRotation();
    m_selectedObject->GetTransform().setRotation
                    (ngl::Vec3{ static_cast<ngl::Real>(val), rot.m_y, rot.m_z });
    update();
  }
}

void NGLScene::setRotY(double val)
{
  if(m_selectedObject)
  {
    ngl::Vec3 rot = m_selectedObject->GetTransform().getRotation();
    m_selectedObject->GetTransform().setRotation
                    (ngl::Vec3{ rot.m_x, static_cast<ngl::Real>(val), rot.m_z });

    update();
  }
}

void NGLScene::setRotZ(double val)
{
  if(m_selectedObject)
  {
    ngl::Vec3 rot = m_selectedObject->GetTransform().getRotation();
    m_selectedObject->GetTransform().setRotation
                    (ngl::Vec3{ rot.m_x, rot.m_y, static_cast<ngl::Real>(val) });
    update();
  }
}

void NGLScene::setScaleX(double val)
{
  if(m_selectedObject)
  {
    ngl::Vec3 scale = m_selectedObject->GetTransform().getScale();
    m_selectedObject->GetTransform().setScale
                    (static_cast<ngl::Real>(val), scale.m_y, scale.m_z);
    update();
  }
}

void NGLScene::setScaleY(double val)
{
  if(m_selectedObject)
  {
    ngl::Vec3 scale = m_selectedObject->GetTransform().getScale();
    m_selectedObject->GetTransform().setScale
                    (scale.m_x, static_cast<ngl::Real>(val), scale.m_z);
    update();
  }
}

void NGLScene::setScaleZ(double val)
{
  if(m_selectedObject)
  {
    ngl::Vec3 scale = m_selectedObject->GetTransform().getScale();
    m_selectedObject->GetTransform().setScale
                  (scale.m_x, scale.m_y, static_cast<ngl::Real>(val));

    update();
  }
}

void NGLScene::OnAddMesh(const std::string& path)
{
  m_sceneObjects.push_back(std::make_shared<MeshObject>(path));
  auto lastSlashPos = path.find_last_of('/');
  auto lastDotPos = path.find_last_of('.');
  m_sceneObjects[m_sceneObjects.size() - 1]->SetName(path.substr(lastSlashPos + 1, lastDotPos - (lastSlashPos + 1)));
  emit UpdateSceneListUI(m_sceneObjects);
  update();
}

void NGLScene::OnSceneListItemSelected(int index)
{
  std::cout<<"SELECTED "<<index<<"\n";
  m_selectedObject = m_sceneObjects[index];
  emit UpdateTransformUI(m_selectedObject->GetTransform());
}

void NGLScene::OnSceneListItemDeleted(int index)
{
  std::cout<<"DELETED "<<index<<"\n";
  if(m_selectedObject == m_sceneObjects[index])
  {
    m_selectedObject = nullptr;
  }
  m_sceneObjects.erase(m_sceneObjects.begin() + index);
  update();
}
