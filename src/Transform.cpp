#include "ngl/ShaderLib.h"
#include "Transform.h"
//----------------------------------------------------------------------------------------------------------------------
/// @file Transform.cpp
/// @brief implementation files for Transform class
//----------------------------------------------------------------------------------------------------------------------
// Constructor -------------------------------------------------------------------------------------------------------------------
Transform::Transform() noexcept
{
  m_position = ngl::Vec3(0.0f,0.0f,0.0f);
  m_scale = ngl::Vec3(1.0f,1.0f,1.0f);
  m_rotation = ngl::Vec3(0.0f,0.0f,0.0f);
  m_isMatrixComputed = false;
  m_matrix=1.0f;
  m_transposeMatrix=1.0f;
  m_inverseMatrix=1.0f;
  computeMatrices();

}

Transform::Transform(const Transform &_t) noexcept
{
  //m_isMatrixComputed=false;

  this->m_position=_t.m_position;
  this->m_scale = _t.m_scale;
  this->m_rotation = _t.m_rotation;
  this->m_isMatrixComputed = true;
  this->m_matrix=_t.m_matrix;
  this->m_transposeMatrix=_t.m_transposeMatrix;
  this->m_inverseMatrix=_t.m_inverseMatrix;
}

Transform & Transform::operator =(const Transform &_t) noexcept
{
  //m_isMatrixComputed=false;

  this->m_position=_t.m_position;
  this->m_scale = _t.m_scale;
  this->m_rotation = _t.m_rotation;
  this->m_isMatrixComputed = true;
  this->m_matrix=_t.m_matrix;
  this->m_transposeMatrix=_t.m_transposeMatrix;
  this->m_inverseMatrix=_t.m_inverseMatrix;
  return *this;
}




void Transform::setMatrix( const ngl::Mat4 &_m   ) noexcept
{
  m_matrix=_m;
  m_transposeMatrix=_m;
  m_transposeMatrix.transpose();
  m_isMatrixComputed = true;
}

// Set scale ---------------------------------------------------------------------------------------------------------------------
void Transform::setScale( const ngl::Vec3& _scale  ) noexcept
{
  m_scale = _scale;
  m_isMatrixComputed = false;
}

void Transform::setScale( const ngl::Vec4& _scale  ) noexcept
{
  m_scale = _scale;
  m_isMatrixComputed = false;
}

void Transform::setScale(ngl::Real _x, ngl::Real _y, ngl::Real _z  ) noexcept
{
  m_scale.set(_x,_y,_z);
  m_isMatrixComputed = false;
}

// add scale ---------------------------------------------------------------------------------------------------------------------
void Transform::addScale( const ngl::Vec3& _scale ) noexcept
{
  m_scale += _scale;
  m_isMatrixComputed = false;
}


void Transform::addScale(ngl::Real _x,  ngl::Real _y,  ngl::Real _z ) noexcept
{
  m_scale.m_x+=_x;
  m_scale.m_y+=_y;
  m_scale.m_z+=_z;

  m_isMatrixComputed = false;
}

// Set position --------------------------------------------------------------------------------------------------------------------
void Transform::setPosition(const ngl::Vec4 &_position ) noexcept
{
  m_position = _position;
  m_isMatrixComputed = false;
}
void Transform::setPosition(const ngl::Vec3 &_position) noexcept
{
  m_position = _position;
  m_isMatrixComputed = false;
}
void Transform::setPosition(ngl::Real _x, ngl::Real _y, ngl::Real _z  ) noexcept
{
  m_position.set(_x,_y,_z);
  m_isMatrixComputed = false;
}

// Set position --------------------------------------------------------------------------------------------------------------------
void Transform::addPosition( const ngl::Vec3& _position) noexcept
{
  m_position+= _position;
  m_isMatrixComputed = false;
}
void Transform::addPosition( ngl::Real _x, ngl::Real _y, ngl::Real _z ) noexcept
{
  m_position.m_x+=_x;
  m_position.m_y+=_y;
  m_position.m_z+=_z;

  m_isMatrixComputed = false;
}


// set rotation -------------------------------------------------------------------------------------------------------------------
void Transform::setRotation( const ngl::Vec3 &_rotation ) noexcept
{
  m_rotation = _rotation;
  m_isMatrixComputed = false;
}
void Transform::setRotation( const ngl::Vec4 &_rotation ) noexcept
{
  m_rotation = _rotation;
  m_isMatrixComputed = false;
}


void Transform::setRotation(ngl::Real _x, ngl::Real _y, ngl::Real _z ) noexcept
{
  m_rotation.set(_x,_y,_z);

  m_isMatrixComputed = false;
}


// set rotation -------------------------------------------------------------------------------------------------------------------
void Transform::addRotation(const ngl::Vec3 &_rotation  ) noexcept
{
  m_rotation+= _rotation;
  m_isMatrixComputed = false;
}

void Transform::addRotation(const glm::quat& quat  ) noexcept
{
  glm::vec3 euler = glm::eulerAngles(quat);
  //std::cout<<quat.length()<<"\n";
  m_rotation.m_x += ngl::degrees(euler.x);
  m_rotation.m_y += ngl::degrees(euler.y);
  m_rotation.m_z += ngl::degrees(euler.z);
  m_isMatrixComputed = false;
}
void Transform::addRotation(const glm::vec3& axis, float angle   ) noexcept
{
  glm::mat4 matrix = glm::mat4(1.0f);
  matrix = glm::rotate(matrix, ngl::degrees(angle), axis);
;
  //std::cout<<quat.length()<<"\n";
  //m_rotation.m_x += ngl::degrees(euler.x);
  //m_rotation.m_y += ngl::degrees(euler.y);
  //m_rotation.m_z += ngl::degrees(euler.z);
  m_isMatrixComputed = false;
}

void Transform::addRotation(ngl::Real _x, ngl::Real _y, ngl::Real _z) noexcept
{
  m_rotation.m_x+=_x;
  m_rotation.m_y+=_y;
  m_rotation.m_z+=_z;
  m_isMatrixComputed = false;
}


// reset matrix ---------------------------------------------------------------------------------------------------------------------
void Transform::reset() noexcept
{
  m_position = ngl::Vec3(0.0f,0.0f,0.0f);
  m_scale = ngl::Vec3(1.0f,1.0f,1.0f);
  m_rotation = ngl::Vec3(0.0f,0.0f,0.0f);
  m_isMatrixComputed = false;
  computeMatrices();
}

// comptue matrix ---------------------------------------------------------------------------------------------------------------------
void Transform::computeMatrices() noexcept
{
  if (!m_isMatrixComputed)       // need to recalculate
  {
    ngl::Mat4 scale;
    ngl::Mat4 rX;
    ngl::Mat4 rY;
    ngl::Mat4 rZ;
    ngl::Mat4 trans;

    // rotation/scale matrix
    ngl::Mat4 rotationScale;
    scale.scale(m_scale.m_x, m_scale.m_y, m_scale.m_z);

    //rX.rotateX(m_rotation.m_x);
    //rY.rotateY(m_rotation.m_y);
    //rZ.rotateZ(m_rotation.m_z);
    glm::mat4 rotationGLM =
          glm::toMat4(glm::quat(glm::vec3(ngl::radians(m_rotation.m_x), ngl::radians(m_rotation.m_y), ngl::radians(m_rotation.m_z) )));

    //rotationGLM = glm::eulerAngleXYZ(glm::radians(m_rotation.m_x), glm::radians(m_rotation.m_y), glm::radians(m_rotation.m_z));
    //rotationGLM = glm::yawPitchRoll(glm::radians(m_rotation.m_x), glm::radians(m_rotation.m_y), glm::radians(m_rotation.m_z));
    ngl::Mat4 rotation;
    memcpy(rotation.m_m,glm::value_ptr(rotationGLM) ,sizeof(rotation.m_m));

    rotationScale = rotation * scale;
    //glm::mat4 tra
    // transform matrix
    m_matrix = rotationScale;
    m_matrix.m_m[3][0] = m_position.m_x;
    m_matrix.m_m[3][1] = m_position.m_y;
    m_matrix.m_m[3][2] = m_position.m_z;
    m_matrix.m_m[3][3] = 1.0f;



    // tranpose matrix
    m_transposeMatrix = rotationScale;
    m_transposeMatrix.transpose();
    m_transposeMatrix.m_m[0][3] = m_position.m_x;
    m_transposeMatrix.m_m[1][3] = m_position.m_y;
    m_transposeMatrix.m_m[2][3] = m_position.m_z;
    m_transposeMatrix.m_m[3][3] = 1;

    // inverse matrix
    trans.translate(-m_position.m_x, -m_position.m_y, -m_position.m_z);
    scale.scale(1.0f / m_scale.m_x, 1.0f / m_scale.m_y, 1.0f / m_scale.m_z);
    rX.rotateX(-m_rotation.m_x);
    rY.rotateY(-m_rotation.m_y);
    rZ.rotateZ(-m_rotation.m_z);
    m_inverseMatrix = trans * rZ * rY * rX * scale  ;

    m_isMatrixComputed = true;
  }
}

void Transform::operator*= ( const Transform &_m) noexcept
{
  m_isMatrixComputed=false;

  computeMatrices();
  m_matrix*=_m.m_matrix;

  /// transpose matrix transformation
  m_transposeMatrix*=_m.m_transposeMatrix;

  /// inverse matrix transformation
  m_inverseMatrix*=_m.m_inverseMatrix;
}

Transform Transform::operator*(const Transform &_m) noexcept
{
  m_isMatrixComputed=false;
  computeMatrices();
  Transform t;
  t.m_matrix=m_matrix*_m.m_matrix;
  t.m_transposeMatrix=m_transposeMatrix*_m.m_transposeMatrix;
  t.m_inverseMatrix=m_inverseMatrix*_m.m_inverseMatrix;

  return t;
}