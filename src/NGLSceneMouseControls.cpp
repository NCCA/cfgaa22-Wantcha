#include "NGLScene.h"
#include <QMouseEvent>


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent( QMouseEvent* _event )
{
  // note the method buttons() is the button state when event was called
  // that is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  m_hoveredObjectID = m_viewportFrameBuffer->ReadPixel( 1 , _event->x(), m_win.height - _event->y() );
  

  if (_event->buttons() == Qt::LeftButton )
  {
    float diffx = _event->x() - m_win.origX;
    float diffy = _event->y() - m_win.origY;
    //m_win.spinXFace += static_cast<int>( 0.5f * diffy );
    //m_win.spinYFace += static_cast<int>( 0.5f * diffx );
    m_win.origX = _event->x();
    m_win.origY = _event->y();
    if(m_win.rotate)
    {
      m_camera->MouseRotate( ngl::Vec2(diffx, diffy ));
      m_win.rotating = true;
    }
    
    else if (m_gizmo->IsManipulating())
    {
      m_gizmo->Manipulate(m_selectedObject->GetTransform(), diffx, diffy);
      emit UpdateTransformUI(m_selectedObject->GetTransform());

      if(m_selectedObject->IsLight())
        PBRShaderManager::RefreshCurrentLights();
    }
  }

  // right mouse translate code
  else if ( m_win.translate && _event->buttons() == Qt::RightButton )
  {
    float diffX      = static_cast<float>( _event->x() - m_win.origXPos );
    float diffY      = static_cast<float>( _event->y() - m_win.origYPos );
    m_win.origXPos = _event->x();
    m_win.origYPos = _event->y();

    m_camera->MousePan( ngl::Vec2(diffX, diffY ));

    //m_modelPos.m_x += INCREMENT * diffX;
    //m_modelPos.m_y -= INCREMENT * diffY;
    //update();
  }
  update();
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent( QMouseEvent* _event )
{

  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  m_win.origX = _event->x();
  m_win.origY = _event->y();

  if ( _event->button() == Qt::LeftButton/* && _event->modifiers() == Qt::AltModifier*/)
  {
    //std::cout<<m_viewportFrameBuffer->ReadPixel( 1, _event->x(), m_win.height - _event->y() ) << " at "<< _event->x() << " "<< _event->y() << "\n";

    if(m_hoveredObjectID < 0 && m_hoveredObjectID > -100)
    {
      m_gizmo->StartManipulate(m_selectedObject->GetTransform(), -(m_hoveredObjectID + 1), ngl::Vec2( m_win.origX, m_win.origY ));
      //std::cout<<-(m_hoveredObjectID + 1)<<"\n";
    }
    else
    {
      m_win.origX  = _event->x();
      m_win.origY  = _event->y();
      m_win.rotate = true;
    }
  }
  // right mouse translate mode
  else if ( _event->button() == Qt::RightButton/* && _event->modifiers() == Qt::AltModifier*/)
  {
    m_win.origXPos  = _event->x();
    m_win.origYPos  = _event->y();
    m_win.translate = true;
  }
  update();
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent( QMouseEvent* _event )
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if ( _event->button() == Qt::LeftButton )
  {
    m_win.rotate = false;
    if(!m_win.rotating && !m_gizmo->IsManipulating())
    {
      if(m_hoveredObjectID == -100)
      {
        m_selectedObject = nullptr;
        emit UpdateTransformUI(Transform());
        emit UpdatePropertiesBox(new QGridLayout());
      }
      else if(m_hoveredObjectID >= 0 && m_hoveredObjectID < m_sceneObjects.size())
      {
        m_selectedObject = m_sceneObjects[m_hoveredObjectID];
        if(m_selectedObject)
        {
          Transform trans = m_selectedObject->GetTransform();
          trans.setScale(ngl::Vec3{1,1,1});
          m_gizmo->SetTransform(trans);
        }
        emit UpdateTransformUI(m_selectedObject->GetTransform());
        emit UpdatePropertiesBox(m_selectedObject->GetLayout());
      }
    }
    

    m_win.rotating = false;
    m_gizmo->StopManipulate();
  }
  // right mouse translate mode
  if ( _event->button() == Qt::RightButton )
  {
    m_win.translate = false;
  }
  update();
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent( QWheelEvent* _event )
{

  // check the diff of the wheel position (0 means no change)

  m_camera->MouseZoom(_event->angleDelta().y());

  /*if ( _event->angleDelta().y() > 0 )
  {
    m_modelPos.m_z += ZOOM;
  }
  else if ( _event->angleDelta().y() < 0 )
  {
    m_modelPos.m_z -= ZOOM;
  }*/
  update();
}
