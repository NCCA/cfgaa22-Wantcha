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
#include "SceneSerializer.h"
#include <Lights.h>
#include "MainWindow.h"

NGLScene::NGLScene()
{
  setFocusPolicy(Qt::FocusPolicy::StrongFocus);
  setMouseTracking(true);

  m_camera = std::make_shared<Camera>(45.0f, 1600.0f / 900.0f, 0.01f, 1000.0f);
  QSurfaceFormat format;
  format.setSamples(4);
  #if defined( __APPLE__)
  format.setMajorVersion(4);
  format.setMinorVersion(1);
  #else
  format.setMajorVersion(4);
  format.setMinorVersion(3);
  #endif
  format.setProfile(QSurfaceFormat::CoreProfile);
  this->setFormat(format);

  AssetManager::RegisterCache(std::make_unique<AssetCache<ObjMesh>>());
  AssetManager::RegisterCache(std::make_unique<AssetCache<ngl::Texture>>());
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
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  FramebufferSpecification fbSpec;
	fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
	fbSpec.Width = 1600;
	fbSpec.Height = 900;
	m_viewportFrameBuffer = std::make_unique<FrameBuffer>(fbSpec);

  ngl::ShaderLib::loadShader("SimpleDepth", "shaders/SimpleDepthVert.glsl", "shaders/SimpleDepthFrag.glsl");
  ngl::ShaderLib::loadShader("LinearDepth", "shaders/LinearDepthVert.glsl", "shaders/LinearDepthFrag.glsl", "shaders/LinearDepthGeo.glsl");
  ngl::ShaderLib::loadShader("SimpleTexture", "shaders/SimpleTextureVert.glsl", "shaders/SimpleTextureFrag.glsl");
  ngl::ShaderLib::loadShader("HDRToCubemap", "shaders/HDRToCubeVert.glsl", "shaders/HDRToCubeFrag.glsl");
  ngl::ShaderLib::loadShader("Skybox", "shaders/SkyboxVert.glsl", "shaders/SkyboxFrag.glsl");
  ngl::ShaderLib::loadShader("Iradiance", "shaders/HDRToCubeVert.glsl", "shaders/ConvoluteCubeMapFrag.glsl");
  ngl::ShaderLib::loadShader("PrefilterHDR", "shaders/HDRToCubeVert.glsl", "shaders/PrefilterHDRFrag.glsl");
  ngl::ShaderLib::loadShader("BRDF", "shaders/SimpleTextureVert.glsl", "shaders/BRDFFrag.glsl");
  ngl::ShaderLib::loadShader("StencilTest", "shaders/StencilColorVert.glsl", "shaders/StencilColorFrag.glsl");

  PBRShaderManager::Init("PBR", "shaders/PBRVert.glsl", "shaders/PBRFrag.glsl");

  m_gizmo = std::make_unique<Gizmo>( m_camera );

  // For testing purposes
  /*m_sceneObjects.push_back(std::make_shared<MeshObject>("meshes/yuri.obj"));
  //static_cast<MeshObject>(m_sceneObjects[0])->GetMesh()->GetMaterial().SetTexture("textures/checkerboard.jpg");
  m_sceneObjects[0]->SetPosition({0.1f, 0.23f, -1.6f});
  m_sceneObjects[0]->SetScale({0.75f, 0.75f, 0.75f});
  m_sceneObjects[0]->GetMaterial().SetTexture(TextureType::ALBEDO, AssetManager::GetAsset<ngl::Texture>("textures/StoneCladding/TexturesCom_Brick_StoneCladding6_1K_albedo.tif"));
  //m_sceneObjects[0]->GetMesh()->GetMaterial().SetTexture(TextureType::ALBEDO, "textures/checkerboard.jpg");
  m_sceneObjects[0]->GetMaterial().SetTexture(TextureType::ROUGHNESS, AssetManager::GetAsset<ngl::Texture>("textures/StoneCladding/TexturesCom_Brick_StoneCladding6_1K_roughness.tif"));
  m_sceneObjects[0]->GetMaterial().SetTexture(TextureType::NORMAL, AssetManager::GetAsset<ngl::Texture>("textures/StoneCladding/TexturesCom_Brick_StoneCladding6_1K_normal.tif"));
  m_sceneObjects[0]->GetMaterial().SetTexture(TextureType::AO, AssetManager::GetAsset<ngl::Texture>("textures/StoneCladding/TexturesCom_Brick_StoneCladding6_1K_ao.tif"));
  //m_selectedObject = m_sceneObjects[0];


  m_sceneObjects.push_back(std::make_shared<MeshObject>("meshes/cerberus_gun.obj"));
  m_sceneObjects[1]->SetPosition({0.0f, 0.5f, 0.0f});
  m_sceneObjects[1]->SetScale({0.75f, 0.75f, 0.75f});
  m_sceneObjects[1]->SetName("Buncf");
  m_sceneObjects[1]->GetMaterial().SetTexture(TextureType::ALBEDO, AssetManager::GetAsset<ngl::Texture>("textures/Cerberus/Cerberus_A.png"));
  m_sceneObjects[1]->GetMaterial().SetTexture(TextureType::ROUGHNESS, AssetManager::GetAsset<ngl::Texture>("textures/Cerberus/Cerberus_R.png"));
  m_sceneObjects[1]->GetMaterial().SetTexture(TextureType::NORMAL, AssetManager::GetAsset<ngl::Texture>("textures/Cerberus/Cerberus_N.png"));
  m_sceneObjects[1]->GetMaterial().SetTexture(TextureType::AO, AssetManager::GetAsset<ngl::Texture>("textures/Cerberus/Cerberus_AO.png"));
  m_sceneObjects[1]->GetMaterial().SetTexture(TextureType::METALLIC, AssetManager::GetAsset<ngl::Texture>("textures/Cerberus/Cerberus_M.png"));

  m_sceneObjects.push_back(std::make_shared<MeshObject>("meshes/plane.obj"));
  m_sceneObjects[2]->SetPosition({0.0f, -0.6f, 0.0f});
  m_sceneObjects[2]->SetScale({5.0f, 1.25f, 5.0f});
  m_sceneObjects[2]->SetName("Plane");

  m_sceneObjects.push_back(PBRShaderManager::AddDirectionalLight(ngl::Vec3{0, 2.0f, 0.5f}, ngl::Vec3{ 135, 0 ,0 }, ngl::Vec3{1.0f, 1.0f, 1.0f}, 0.5f));
  m_sceneObjects.push_back(PBRShaderManager::AddDirectionalLight(ngl::Vec3{1.0f, 2.0f, 0.5f}, ngl::Vec3{ 45, 90 ,0 }, ngl::Vec3{1.0f, 0.7f, 0.8f}, 0.5f));
  m_sceneObjects.push_back(PBRShaderManager::AddPointLight(ngl::Vec3{-1, 2, 0}, ngl::Vec3{1, 0.5f, 0.7f}, 5.0f));
  m_sceneObjects.push_back(PBRShaderManager::AddPointLight(ngl::Vec3(0.5f, 2.0f, -1.0f), ngl::Vec3{1,1,1}, 2.0f));*/

  m_environment = std::make_unique<EnvironmentTexture>( PBRShaderManager::s_whiteTextureID );

  emit UpdateSceneListUI(m_sceneObjects);
  emit UpdateTransformUI(Transform());
  emit UpdatePropertiesBox(nullptr);
}


void NGLScene::paintGL()
{
  makeCurrent();
  std::vector<ngl::Mat4> directionalLightSpaceMats;
  // Render shadow maps
  glEnable(GL_CULL_FACE);  
  glCullFace(GL_FRONT);
  for(int i = 0; i < PBRShaderManager::s_directionalLights.size() && i < PBRShaderManager::s_maxDirectionalShadows; i++)
  {
    ngl::Mat4 lightSpaceMat = PBRShaderManager::s_directionalLights[i]->GetProjection() * PBRShaderManager::s_directionalLights[i]->GetView();
    directionalLightSpaceMats.push_back(lightSpaceMat);
    PBRShaderManager::s_directionalLights[i]->GetShadowBuffer()->Bind();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glViewport(0,0,PBRShaderManager::s_shadowMapSize, PBRShaderManager::s_shadowMapSize);
    glClear(GL_DEPTH_BUFFER_BIT);
    ngl::ShaderLib::use("SimpleDepth");
    for(auto mesh : m_sceneObjects)
    {
      if(!mesh->IsLight())
      {
        ngl::Mat4 MVP = lightSpaceMat * mesh->GetTransform().getMatrix();      
        ngl::ShaderLib::setUniform("MVP", MVP);
        mesh->Draw();
      }
    }

    PBRShaderManager::s_directionalLights[i]->GetShadowBuffer()->Unbind();
  }

  // POINT LIGHT SHADOWS
  PBRShaderManager::s_pointShadowBuffer->Bind();
  glViewport(0,0,PBRShaderManager::s_shadowMapSize, PBRShaderManager::s_shadowMapSize);
  glClear(GL_DEPTH_BUFFER_BIT);
  ngl::ShaderLib::use("LinearDepth");
  ngl::ShaderLib::setUniform("far_plane", 25.0f);
  ngl::ShaderLib::setUniform("cubemapCount", int(PBRShaderManager::s_pointLights.size()));

  for(int i = 0; i < PBRShaderManager::s_pointLights.size() && i < PBRShaderManager::s_maxPointShadows; i++)
  {
    std::array<ngl::Mat4, 6> shadowTransforms;
    ngl::Mat4 projection = PBRShaderManager::s_pointLights[i]->GetProjection();
    ngl::Vec3 position = PBRShaderManager::s_pointLights[i]->GetTransform().getPosition();
    shadowTransforms[0] = projection * ngl::lookAt( position, position + ngl::Vec3( 1, 0, 0 ), ngl::Vec3(0, -1, 0) );
    shadowTransforms[1] = projection * ngl::lookAt( position, position + ngl::Vec3( -1, 0, 0 ), ngl::Vec3(0, -1, 0) );
    shadowTransforms[2] = projection * ngl::lookAt( position, position + ngl::Vec3( 0, 1, 0 ), ngl::Vec3(0, 0, 1) );
    shadowTransforms[3] = projection * ngl::lookAt( position, position + ngl::Vec3( 0, -1, 0 ), ngl::Vec3(0, 0, -1) );
    shadowTransforms[4] = projection * ngl::lookAt( position, position + ngl::Vec3( 0, 0, 1 ), ngl::Vec3(0, -1, 0) );
    shadowTransforms[5] = projection * ngl::lookAt( position, position + ngl::Vec3( 0, 0, -1 ), ngl::Vec3(0, -1, 0) );
    for(int j = 0; j < 6; j++)
    {
      ngl::ShaderLib::setUniform("PointSpaceMats[" + std::to_string(i * 6 + j) + "]", shadowTransforms[j]);
    }
    ngl::ShaderLib::setUniform("pLightPositions[" + std::to_string(i) + "]", position);
  }
  for(auto mesh : m_sceneObjects)
  {
    if(!mesh->IsLight())
    {
      ngl::ShaderLib::setUniform("model", mesh->GetTransform().getMatrix());     
      mesh->Draw();
    }
  }
  PBRShaderManager::s_pointShadowBuffer->Unbind();


  // Render the main scene
  m_viewportFrameBuffer->Bind();
  // clear the screen and depth buffer
  glClearColor(m_backgroundColor.m_r, m_backgroundColor.m_g, m_backgroundColor.m_b, 1.0f);	 //Background
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glViewport(0, 0, m_win.width, m_win.height);
  glCullFace(GL_BACK);

  m_viewportFrameBuffer->ClearAttachment(1, -100);

  int it = 0;
  
  ngl::Mat4 VP = m_camera->GetProjection() * m_camera->GetView();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_STENCIL_TEST);
  for(auto mesh : m_sceneObjects)
  {
    if(m_selectedObject == mesh)
    {
      glStencilFunc(GL_ALWAYS, 1, 0xFF);
      glStencilMask(0xFF);
    }
    else
    {
      //glStencilFunc(GL_ALWAYS, 1, 0x00);
      glStencilMask(0x00);
    }
    ngl::Mat4 MVP = VP * mesh->GetTransform().getMatrix();
    if(!mesh->IsLight())
    {
      PBRShaderManager::UseShader();
      ngl::ShaderLib::setUniform("camPos", m_camera->GetTransform().getPosition());
      ngl::ShaderLib::setUniform("MVP", MVP);
      ngl::ShaderLib::setUniform("ambientIntensity", m_ambientIntensity);
      ngl::ShaderLib::setUniform("roughnessValue", mesh->GetMaterial().m_roughness);
      ngl::ShaderLib::setUniform("metallicValue", mesh->GetMaterial().m_metallic);
      ngl::ShaderLib::setUniform("baseColor", mesh->GetMaterial().m_baseColor);
      ngl::ShaderLib::setUniform("M", mesh->GetTransform().getMatrix());
      for(int i = 0; i < PBRShaderManager::s_directionalLights.size() && i < PBRShaderManager::s_maxDirectionalShadows; i++)
      {
        ngl::ShaderLib::setUniform("LightSpaceMats[" + std::to_string(i) + "]", directionalLightSpaceMats[i]);
      }
      
      glActiveTexture(GL_TEXTURE6);
      glBindTexture(GL_TEXTURE_2D_ARRAY, PBRShaderManager::s_directionalShadowMap);
      glActiveTexture(GL_TEXTURE7);
      glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, PBRShaderManager::s_pointShadowMap);
      glActiveTexture(GL_TEXTURE8);
      glBindTexture(GL_TEXTURE_CUBE_MAP, m_environment->GetIradianceMap());
      glActiveTexture(GL_TEXTURE9);
      glBindTexture(GL_TEXTURE_CUBE_MAP, m_environment->GetPrefilteredMap());
      glActiveTexture(GL_TEXTURE10);
      glBindTexture(GL_TEXTURE_2D, m_environment->GetBRDFMap());
      
      ngl::ShaderLib::setUniform("objectID", it);

      mesh->GetMaterial().BindTextures();
      mesh->Draw();
    }
    else
    {
      ngl::ShaderLib::use("UnlitConstantSize");
      ngl::ShaderLib::setUniform("MVP", MVP);
      ngl::ShaderLib::setUniform("reciprScaleOnScreen", 2.0f * 50 / m_win.width);
      ngl::ShaderLib::setUniform("Color", ngl::Vec4{1.0f, 0.95f, 0.8f, 1.0f});
      ngl::ShaderLib::setUniform("objectID", it);
      mesh->DrawHighlighted();
    }
    
    ++it;
  }

  glDisable(GL_CULL_FACE);  

  // Now draw selection borders
  if(m_selectedObject && !m_selectedObject->IsLight())
  {
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    //glDisable(GL_DEPTH_TEST);
    ngl::ShaderLib::use("StencilTest");
    ngl::ShaderLib::setUniform("MVP", VP * m_selectedObject->GetTransform().getMatrix());
    m_selectedObject->Draw();
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    //glEnable(GL_DEPTH_TEST);
  }

  if(m_renderEnvironment)
  {
    glDepthFunc(GL_LEQUAL);
    ngl::ShaderLib::use("Skybox");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_environment->GetEnvironmentCubeMap());
    ngl::ShaderLib::setUniform("projection", m_camera->GetProjection());
    ngl::ShaderLib::setUniform("view", m_camera->GetView());
    m_environment->GetCube()->Draw();

    glDepthFunc(GL_LESS);
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
  switch (_event->key())
  {
    case Qt::Key_W :
      m_gizmo->SetType(GizmoType::TRANSLATE); break;

  case Qt::Key_R :
      m_gizmo->SetType(GizmoType::ROTATE); break;
  case Qt::Key_E :
      m_gizmo->SetType(GizmoType::SCALE); break;
  case Qt::Key_Q :
      m_gizmo->SetType(GizmoType::NONE); break;
  case Qt::Key_F :
      if(m_selectedObject) m_camera->Refocus(m_selectedObject->GetTransform()); break;

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
  makeCurrent();
  m_sceneObjects.push_back(std::make_shared<MeshObject>(path));
  auto lastSlashPos = path.find_last_of('/');
  auto lastDotPos = path.find_last_of('.');
  m_sceneObjects[m_sceneObjects.size() - 1]->SetName(path.substr(lastSlashPos + 1, lastDotPos - (lastSlashPos + 1)));
  emit UpdateSceneListUI(m_sceneObjects);
  update();
  //doneCurrent();
}

void NGLScene::OnAddDirectionalLight()
{
  makeCurrent();
  m_sceneObjects.push_back(PBRShaderManager::AddDirectionalLight());
  emit UpdateSceneListUI(m_sceneObjects);
  update();
  //doneCurrent();
}

void NGLScene::OnAddPointLight()
{
  makeCurrent();
  m_sceneObjects.push_back(PBRShaderManager::AddPointLight());
  emit UpdateSceneListUI(m_sceneObjects);
  update();
  //doneCurrent();
}

void NGLScene::OnSceneListItemSelected(int index)
{
  std::cout<<"SELECTED "<<index<<"\n";
  m_selectedObject = m_sceneObjects[index];
  emit UpdateTransformUI(m_selectedObject->GetTransform());
  emit UpdatePropertiesBox(m_selectedObject);
  //emit UpdatePropertiesUI(m_selectedObject);
}

void NGLScene::OnSceneListItemDeleted(int index)
{
  std::cout<<"DELETED "<<index<<"\n";
  if(m_selectedObject == m_sceneObjects[index])
  {
    m_selectedObject = nullptr;
    emit UpdateTransformUI(Transform());
    emit UpdatePropertiesBox(nullptr);
  }
  std::vector<std::shared_ptr<SceneObject>>::iterator iterator = m_sceneObjects.begin() + index;
  std::shared_ptr<SceneObject> object = *iterator;
  if(object->IsLight())
  {
    std::shared_ptr<Light> light = std::dynamic_pointer_cast<Light>(object);
    if(light->GetType() == LightType::Directional)
    {
      PBRShaderManager::RemoveDirectionalLight(light);
    }
    else
    {
      PBRShaderManager::RemovePointLight(light);
    }
  }

  m_sceneObjects.erase(iterator);
  update();
  //AssetManager::CollectGarbage();
}

void NGLScene::OnSaveFramebuffer(const std::string& path)
{
  RenderFramebuffer(m_win.width, m_win.height);

  m_viewportFrameBuffer->SaveFramebufferToPNG(0, path, 0, 0,
            m_viewportFrameBuffer->GetSpecification().Width, m_viewportFrameBuffer->GetSpecification().Height);
}

void NGLScene::OnSaveFramebufferSize(const std::string& filepath, int width, int height)
{
  RenderFramebuffer(width, height);

  m_viewportFrameBuffer->SaveFramebufferToPNG(0, filepath, 0, 0,
            width, height);

  m_viewportFrameBuffer->Resize(m_win.width, m_win.height);
}

void NGLScene::resetScene()
{
  while(m_sceneObjects.size() != 0)
  {
      OnSceneListItemDeleted(0);
  }
  emit UpdateSceneListUI(m_sceneObjects);
  AssetManager::CollectGarbage();
}

void NGLScene::RenderFramebuffer(int width, int height)
{
  makeCurrent();
  m_camera->SetViewportSize(width, height);
  m_viewportFrameBuffer->Resize(width, height);

  std::vector<ngl::Mat4> directionalLightSpaceMats;
  // Render shadow maps
  glEnable(GL_CULL_FACE);  
  glCullFace(GL_FRONT);
  for(int i = 0; i < PBRShaderManager::s_directionalLights.size() && i < PBRShaderManager::s_maxDirectionalShadows; i++)
  {
    ngl::Mat4 lightSpaceMat = PBRShaderManager::s_directionalLights[i]->GetProjection() * PBRShaderManager::s_directionalLights[i]->GetView();
    directionalLightSpaceMats.push_back(lightSpaceMat);
    PBRShaderManager::s_directionalLights[i]->GetShadowBuffer()->Bind();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glViewport(0,0,PBRShaderManager::s_shadowMapSize, PBRShaderManager::s_shadowMapSize);
    glClear(GL_DEPTH_BUFFER_BIT);
    ngl::ShaderLib::use("SimpleDepth");
    for(auto mesh : m_sceneObjects)
    {
      if(!mesh->IsLight())
      {
        ngl::Mat4 MVP = lightSpaceMat * mesh->GetTransform().getMatrix();      
        ngl::ShaderLib::setUniform("MVP", MVP);
        mesh->Draw();
      }
    }

    PBRShaderManager::s_directionalLights[i]->GetShadowBuffer()->Unbind();
  }

  // POINT LIGHTS
  PBRShaderManager::s_pointShadowBuffer->Bind();
  glViewport(0,0,PBRShaderManager::s_shadowMapSize, PBRShaderManager::s_shadowMapSize);
  glClear(GL_DEPTH_BUFFER_BIT);
  ngl::ShaderLib::use("LinearDepth");
  ngl::ShaderLib::setUniform("far_plane", 25.0f);
  ngl::ShaderLib::setUniform("cubemapCount", int(PBRShaderManager::s_pointLights.size()));

  for(int i = 0; i < PBRShaderManager::s_pointLights.size() && i < PBRShaderManager::s_maxPointShadows; i++)
  {
    std::array<ngl::Mat4, 6> shadowTransforms;
    ngl::Mat4 projection = PBRShaderManager::s_pointLights[i]->GetProjection();
    ngl::Vec3 position = PBRShaderManager::s_pointLights[i]->GetTransform().getPosition();
    shadowTransforms[0] = projection * ngl::lookAt( position, position + ngl::Vec3( 1, 0, 0 ), ngl::Vec3(0, -1, 0) );
    shadowTransforms[1] = projection * ngl::lookAt( position, position + ngl::Vec3( -1, 0, 0 ), ngl::Vec3(0, -1, 0) );
    shadowTransforms[2] = projection * ngl::lookAt( position, position + ngl::Vec3( 0, 1, 0 ), ngl::Vec3(0, 0, 1) );
    shadowTransforms[3] = projection * ngl::lookAt( position, position + ngl::Vec3( 0, -1, 0 ), ngl::Vec3(0, 0, -1) );
    shadowTransforms[4] = projection * ngl::lookAt( position, position + ngl::Vec3( 0, 0, 1 ), ngl::Vec3(0, -1, 0) );
    shadowTransforms[5] = projection * ngl::lookAt( position, position + ngl::Vec3( 0, 0, -1 ), ngl::Vec3(0, -1, 0) );
    for(int j = 0; j < 6; j++)
    {
      ngl::ShaderLib::setUniform("PointSpaceMats[" + std::to_string(i * 6 + j) + "]", shadowTransforms[j]);
    }
    ngl::ShaderLib::setUniform("pLightPositions[" + std::to_string(i) + "]", position);
  }
  for(auto mesh : m_sceneObjects)
  {
    if(!mesh->IsLight())
    {
      ngl::ShaderLib::setUniform("model", mesh->GetTransform().getMatrix());     
      mesh->Draw();
    }
  }
  PBRShaderManager::s_pointShadowBuffer->Unbind();


  m_viewportFrameBuffer->Bind();
  // clear the screen and depth buffer
  glClearColor(0.15f, 0.15f, 0.18f, 1.0f);			   // Grey Background
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, width, height);
  glCullFace(GL_BACK);

  m_viewportFrameBuffer->ClearAttachment(1, -100);

  int it = 0;
  
  ngl::Mat4 VP = m_camera->GetProjection() * m_camera->GetView();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  for(auto mesh : m_sceneObjects)
  {
    ngl::Mat4 MVP = VP * mesh->GetTransform().getMatrix();
    if(!mesh->IsLight())
    {
      PBRShaderManager::UseShader();
      ngl::ShaderLib::setUniform("camPos", m_camera->GetTransform().getPosition());
      ngl::ShaderLib::setUniform("MVP", MVP);
      ngl::ShaderLib::setUniform("ambientIntensity", m_ambientIntensity);
      ngl::ShaderLib::setUniform("M", mesh->GetTransform().getMatrix());
      for(int i = 0; i < PBRShaderManager::s_directionalLights.size() && i < PBRShaderManager::s_maxDirectionalShadows; i++)
      {
        ngl::ShaderLib::setUniform("LightSpaceMats[" + std::to_string(i) + "]", directionalLightSpaceMats[i]);
      }
      
      glActiveTexture(GL_TEXTURE6);
      glBindTexture(GL_TEXTURE_2D_ARRAY, PBRShaderManager::s_directionalShadowMap);
      glActiveTexture(GL_TEXTURE7);
      glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, PBRShaderManager::s_pointShadowMap);
      glActiveTexture(GL_TEXTURE8);
      glBindTexture(GL_TEXTURE_CUBE_MAP, m_environment->GetIradianceMap());
      glActiveTexture(GL_TEXTURE9);
      glBindTexture(GL_TEXTURE_CUBE_MAP, m_environment->GetPrefilteredMap());
      glActiveTexture(GL_TEXTURE10);
      glBindTexture(GL_TEXTURE_2D, m_environment->GetBRDFMap());
      
      ngl::ShaderLib::setUniform("objectID", it);

      mesh->GetMaterial().BindTextures();
      mesh->Draw();
    } 
    ++it;
  }
  glDisable(GL_CULL_FACE);  

  if(m_renderEnvironment)
  {
    glDepthFunc(GL_LEQUAL);
    ngl::ShaderLib::use("Skybox");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_environment->GetEnvironmentCubeMap());
    ngl::ShaderLib::setUniform("projection", m_camera->GetProjection());
    ngl::ShaderLib::setUniform("view", m_camera->GetView());
    m_environment->GetCube()->Draw();

    glDepthFunc(GL_LESS);
  }

  //m_viewportFrameBuffer->BlitToScreen(defaultFramebufferObject());

  m_viewportFrameBuffer->Unbind();

  m_camera->SetViewportSize(m_win.width, m_win.height);
}
