#include <QMouseEvent>
#include <QGuiApplication>
#include <QSurfaceFormat>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Shader.h>
#include <iostream>

#include "MeshObject.h"
#include "Assets/AssetManager.h"
#include <Lights.h>

NGLScene::NGLScene()
{
  setFocusPolicy(Qt::FocusPolicy::StrongFocus);
  setMouseTracking(true);

  m_camera = std::make_shared<Camera>(45.0f, 1600.0f / 900.0f, 0.01f, 1000.0f);
  QSurfaceFormat format;
  format.setSamples(4);
  #if defined( __APPLE__)
  format.setMajorVersion(4);
  format.setMinorVersion(3);
  #else
  format.setMajorVersion(4);
  format.setMinorVersion(3);
  #endif
  format.setProfile(QSurfaceFormat::CoreProfile);
  this->setFormat(format);

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

  m_camera->SetViewportSize(_w, _h);
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
        {0, 0, 0}, { 90, 0 ,0 }, {1.0f, 1.0f, 1.0f}, 2.0f);

  Light l2(LightType::Directional, {0, 0, 0}, { 0, 0, 0 },
            {1.0f, 1.0f, 1.0f}, 0.9f);

  Light l3(LightType::Point, {0, 1, 0}, { 0, 0 ,0 }, {1, 0.9f, 1});

  m_directionalLights.push_back(l1);
  //m_directionalLights.push_back(l2);

  Light pl1(LightType::Point, ngl::Vec3(0.5f, 0.5f, -1.0f), ngl::Vec3(0, 0, 0), {1,1,1});
  Light pl2(LightType::Point, ngl::Vec3(-0.5f, 0.5f, 2.0f), ngl::Vec3(0, 0, 0), {1,1,1});
  m_pointLights.push_back(pl1);
  m_pointLights.push_back(pl2);
  m_pointLights.push_back(l3);

  PBRShaderManager::Init("PBR", "shaders/PBRVert.glsl", "shaders/PBRFrag.glsl");
  PBRShaderManager::UpdateLightCounts(m_directionalLights, m_pointLights);

  m_gizmo = std::make_unique<Gizmo>( m_camera );
  //ngl::ShaderLib::setUniform("directionalLightCount", static_cast<int>(m_directionalLights.size()));


  m_sceneObjects.push_back(std::make_shared<MeshObject>("meshes/yuri.obj"));
  //static_cast<MeshObject>(m_sceneObjects[0])->GetMesh()->GetMaterial().SetTexture("textures/checkerboard.jpg");
  m_sceneObjects[0]->SetPosition({0.1f, 0.23f, -1.6f});
  m_sceneObjects[0]->SetScale({0.75f, 0.75f, 0.75f});
  //m_sceneObjects[0]->GetMesh()->GetMaterial().SetTexture(TextureType::ALBEDO, "textures/StoneCladding/TexturesCom_Brick_StoneCladding6_1K_albedo.tif");
  m_sceneObjects[0]->GetMesh()->GetMaterial().SetTexture(TextureType::ALBEDO, "textures/checkerboard.jpg");
  m_sceneObjects[0]->GetMesh()->GetMaterial().SetTexture(TextureType::ROUGHNESS, "textures/StoneCladding/TexturesCom_Brick_StoneCladding6_1K_roughness.tif");
  //m_sceneObjects[0]->GetMesh()->GetMaterial().SetTexture(TextureType::NORMAL, "textures/StoneCladding/TexturesCom_Brick_StoneCladding6_1K_normal.tif");
  m_sceneObjects[0]->GetMesh()->GetMaterial().SetTexture(TextureType::AO, "textures/StoneCladding/TexturesCom_Brick_StoneCladding6_1K_ao.tif");
  m_selectedObject = m_sceneObjects[0];


  m_sceneObjects.push_back(std::make_shared<MeshObject>("meshes/cerberus_gun.obj"));
  m_sceneObjects[1]->SetPosition({0.0f, 0.5f, 0.0f});
  m_sceneObjects[1]->SetScale({0.75f, 0.75f, 0.75f});
  m_sceneObjects[1]->SetName("Buncf");
  m_sceneObjects[1]->GetMesh()->GetMaterial().SetTexture(TextureType::ALBEDO, "textures/Cerberus/Cerberus_A.png");
  m_sceneObjects[1]->GetMesh()->GetMaterial().SetTexture(TextureType::ROUGHNESS, "textures/Cerberus/Cerberus_R.png");
  m_sceneObjects[1]->GetMesh()->GetMaterial().SetTexture(TextureType::NORMAL, "textures/Cerberus/Cerberus_N.png");
  m_sceneObjects[1]->GetMesh()->GetMaterial().SetTexture(TextureType::AO, "textures/Cerberus/Cerberus_AO.png");
  m_sceneObjects[1]->GetMesh()->GetMaterial().SetTexture(TextureType::METALLIC, "textures/Cerberus/Cerberus_M.png");

  emit UpdateSceneListUI(m_sceneObjects);
  emit UpdateTransformUI(m_selectedObject->GetTransform());
}


void NGLScene::paintGL()
{
  makeCurrent();

  m_viewportFrameBuffer->Bind();
  // clear the screen and depth buffer
  glClearColor(0.15f, 0.15f, 0.18f, 1.0f);			   // Grey Background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_viewportFrameBuffer->ClearAttachment(1, -100);

  int it = 0;
  
  ngl::Mat4 VP = m_camera->GetProjection() * m_camera->GetView();

  PBRShaderManager::UseShader();
  ngl::ShaderLib::setUniform("camPos", m_camera->GetTransform().getPosition());
  for(auto mesh : m_sceneObjects)
  {
    PBRShaderManager::UseShader();
    //ngl::Mat3 normalMatrix = MV.inverse().transpose();
    ngl::Mat4 MVP = VP * mesh->GetTransform().getMatrix();
    //ngl::ShaderLib::setUniform("NormalMatrix", normalMatrix);

    ngl::ShaderLib::setUniform("MVP", MVP);
    ngl::ShaderLib::setUniform("M", mesh->GetTransform().getMatrix());
    //ngl::Mat4 normalMatrix = m_camera->GetView() * mesh->GetTransform().getMatrix();
    //normalMatrix.inverse().transpose();
    //ngl::ShaderLib::setUniform("NormalMatrix", normalMatrix);
    
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
  if(m_selectedObject)
  {
    Transform trans = m_selectedObject->GetTransform();
    trans.setScale(ngl::Vec3{1.0f,1.0f,1.0f});
    trans.getMatrix(); /*recomputing matrix*/
    m_gizmo->SetTransform(trans);
    m_gizmo->Draw(VP, m_win.width);
  }

  m_viewportFrameBuffer->BlitToScreen(defaultFramebufferObject());

  m_viewportFrameBuffer->Unbind();

  doneCurrent();
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
  case Qt::Key_W :
      m_gizmo->SetType(GizmoType::TRANSLATE); break;
  case Qt::Key_R :
      m_gizmo->SetType(GizmoType::ROTATE); break;
  case Qt::Key_E :
      m_gizmo->SetType(GizmoType::SCALE); break;
  case Qt::Key_Q :
      m_gizmo->SetType(GizmoType::NONE); break;

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
  emit UpdatePropertiesBox(m_selectedObject->GetLayout());
  //emit UpdatePropertiesUI(m_selectedObject);
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
