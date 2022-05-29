#ifndef TRANSFORM_H_
#define TRANSFORM_H_
//----------------------------------------------------------------------------------------------------------------------
/// @file Transform.h
/// @brief a simple transformation object containing rot / tx / scale and final matrix
//----------------------------------------------------------------------------------------------------------------------
// Library includes
#include "ngl/Mat4.h"
//#include "NGLassert.h"
//#include "Transformation.h"
#include "ngl/Vec4.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
//----------------------------------------------------------------------------------------------------------------------
/// @enum decide which matrix is the current active matrix
//----------------------------------------------------------------------------------------------------------------------
enum  class ActiveMatrix : char{NORMAL,TRANSPOSE,INVERSE};
//----------------------------------------------------------------------------------------------------------------------
/// @class Transform "include/ngl/Transform.h"
/// @brief Transform describes a transformation (translate, scale, rotation)
/// modifed by j macey and included into NGL
/// @author Vincent Bonnet
/// @version 1.5
/// @date 14/03/10 Last Revision 14/03/10
//----------------------------------------------------------------------------------------------------------------------
class Transform
{
  friend class Vec4;
public:

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Constructor
  //----------------------------------------------------------------------------------------------------------------------
  Transform() noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Copy Constructor
  //----------------------------------------------------------------------------------------------------------------------
  Transform(const Transform &_t) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief assignment operator
  //----------------------------------------------------------------------------------------------------------------------
  Transform & operator =(const Transform &_t) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to set the scale value in the transform
  /// @param[in] _scale the scale value to set for the transform
  //----------------------------------------------------------------------------------------------------------------------
  void setScale( const ngl::Vec3& _scale ) noexcept;
  void setScale( const ngl::Vec4& _scale ) noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to set the scale value in the transform
  /// @param[in] _x x scale value
  /// @param[in] _y y scale value
  /// @param[in] _z z scale value
  //----------------------------------------------------------------------------------------------------------------------
  void setScale(  ngl::Real _x,  ngl::Real _y,  ngl::Real _z  ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to add to the existing the scale value in the transform
  /// @param[in] _scale the scale value to set for the transform
  //----------------------------------------------------------------------------------------------------------------------
  void addScale( const ngl::Vec3& _scale ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to add to the existing the scale value in the transform
  /// @param[in] _x x scale value
  /// @param[in] _y y scale value
  /// @param[in] _z z scale value
  //----------------------------------------------------------------------------------------------------------------------
  void addScale(  ngl::Real _x,  ngl::Real _y, ngl::Real _z ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to set the position
  /// @param[in] _position position
  //----------------------------------------------------------------------------------------------------------------------
  void setPosition( const ngl::Vec4& _position ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to set the position
  /// @param[in] _position position
  //----------------------------------------------------------------------------------------------------------------------
  void setPosition( const ngl::Vec3& _position ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to set the position value in the transform
  /// @param[in] _x x position value
  /// @param[in] _y y position value
  /// @param[in] _z z position value
  //----------------------------------------------------------------------------------------------------------------------
  void setPosition( ngl::Real _x, ngl::Real _y, ngl::Real _z  ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method add to the existing set the position
  /// @param[in] _position position
  //----------------------------------------------------------------------------------------------------------------------
  void addPosition( const ngl::Vec4& _position  ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method add to the existing set the position
  /// @param[in] _position position
  //----------------------------------------------------------------------------------------------------------------------
  void addPosition( const ngl::Vec3& _position ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to add to the existing position value in the transform
  /// @param[in] _x x position value
  /// @param[in] _y y position value
  /// @param[in] _z z position value
  //----------------------------------------------------------------------------------------------------------------------
  void addPosition( ngl::Real _x, ngl::Real _y,  ngl::Real _z  ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @breif method to set the matrix directly
  /// @param[in] _m the matrix to set the m_transform to
  /// need to also re-compute the others
  //----------------------------------------------------------------------------------------------------------------------
  void setMatrix( const ngl::Mat4 &_m ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to set the rotation
  /// @param[in] _rotation rotation
  /// @note each value is an axis rotation as the values are calculated
  /// mRotationX * mRotationY * mRotationZ;
  //----------------------------------------------------------------------------------------------------------------------
  void setRotation( const ngl::Vec3& _rotation ) noexcept;
  void setRotation( const ngl::Vec4& _rotation ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to set the rotation value in the transform
  /// @note each value is an axis rotation as the values are calculated
  /// mRotationX * mRotationY * mRotationZ;
  /// @param[in] _x x rotation value
  /// @param[in] _y y rotation value
  /// @param[in] _z z rotation value
  //----------------------------------------------------------------------------------------------------------------------
  void setRotation( ngl::Real _x, ngl::Real _y, ngl::Real _z ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to add to the existing  rotation
  /// @param[in] _rotation rotation
  /// @note each value is an axis rotation as the values are calculated
  /// mRotationX * mRotationY * mRotationZ;
  //----------------------------------------------------------------------------------------------------------------------
  void addRotation( const ngl::Vec3& _rotation   ) noexcept;
  //void addRotation( const glm::quat& quat   ) noexcept;
  void addRotation( const ngl::Vec3& axis, float angle   ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to add to the existing rotation value in the transform
  /// @note each value is an axis rotation as the values are calculated
  /// mRotationX * mRotationY * mRotationZ;
  /// @param[in] _x x rotation value
  /// @param[in] _y y rotation value
  /// @param[in] _z z rotation value
  //----------------------------------------------------------------------------------------------------------------------
  void addRotation( ngl::Real _x, ngl::Real _y, ngl::Real _z  ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a method to set all the transforms to the identity
  //----------------------------------------------------------------------------------------------------------------------
  void reset() noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief function to get the scale
  /// @returns the scale
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 getScale()  const  noexcept    { return m_scale;  }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief function to get the position
  /// @returns the position
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 getPosition() const  noexcept  { return m_position;  }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief function to get the rotation
  /// @returns the rotation
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 getRotation() const  noexcept  { return m_rotation;  }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief function to get the matrix. It computes the matrix if it's dirty
  /// @returns the matrix
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 getMatrix() noexcept{ computeMatrices();  return m_matrix;  }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief function to get the transpose matrix. It computes the transpose matrix if it's dirty
  /// @returns the transpose matrix
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 getTransposeMatrix() noexcept{  computeMatrices(); return m_transposeMatrix; }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief function to get the inverse matrix. It computes the inverse matrix if it's dirty
  /// @returns the inverse matrix
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 getInverseMatrix() noexcept {  computeMatrices(); return m_inverseMatrix; }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief *= operator
  /// @param _m the transformation to combine
  //----------------------------------------------------------------------------------------------------------------------
  void operator*=( const Transform &_m  ) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  ///  @brief operator for Transform multiplication will do a matrix
  /// multiplication on each of the matrices
  /// @note this is not const as we need to check that the members are
  /// calculated before we do the multiplication. This is deliberate
  /// @param[in] _m the Transform to multiply the current one by
  /// @returns all the transform matrix members * my _m members
  //----------------------------------------------------------------------------------------------------------------------
  Transform operator*( const Transform &_m  ) noexcept;

protected :

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief position
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 m_position;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  scale
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 m_scale;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  rotation
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 m_rotation;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  boolean defines if the matrix is dirty or not
  //----------------------------------------------------------------------------------------------------------------------
  bool m_isMatrixComputed;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  matrix transformation
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_matrix;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  transpose matrix transformation
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_transposeMatrix;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  inverse matrix transformation
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_inverseMatrix;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to compute the matrix, transpose and inverse matrix. set the m_bIsMatrixComputed variable to true.
  //----------------------------------------------------------------------------------------------------------------------
  void computeMatrices() noexcept;

};
#endif