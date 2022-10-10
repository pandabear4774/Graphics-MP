#include "Plane.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

Plane::Plane( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _propAngle = 0.0f;
    _propAngleRotationSpeed = M_PI / 16.0f;

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _rotatePlaneAngle = M_PI / 2.0f;

    _colorBody = glm::vec3( 0.0f, 0.0f, 1.0f );
    _scaleBody = glm::vec3( 2.0f, 0.5f, 1.0f );

    _colorWing = glm::vec3( 1.0f, 0.0f, 0.0f );
    _scaleWing = glm::vec3( 1.5f, 0.5f, 1.0f );
    _scaleGun = glm::vec3( 5.5f, 5.5f, 5.0f );
    _rotateWingAngle = M_PI / 2.0f;

    _colorNose = glm::vec3( 0.0f, 1.0f, 0.0f );
    _rotateNoseAngle = M_PI / 2.0f;

    _colorProp = glm::vec3( 1.0f, 1.0f, 1.0f );
    _scaleProp = glm::vec3( 1.1f, 1.0f, 0.025f );
    _transProp = glm::vec3( 0.1f * 25, 0.0f, 0.0f );

    _colorTail = glm::vec3( 1.0f, 1.0f, 0.0f );

    _planeLocation = glm::vec3(0,10,0);
    _direction = -M_PI;
    speed = 0.1;
}

void Plane::drawPlane( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    glUseProgram( _shaderProgramHandle );

    modelMtx = glm::rotate( modelMtx, -_rotatePlaneAngle, CSCI441::Y_AXIS );
    modelMtx = glm::rotate( modelMtx, _rotatePlaneAngle, CSCI441::Z_AXIS );

    _drawPlaneBody(modelMtx, viewMtx, projMtx);        // the body of our plane
    _drawPlaneWing(true, modelMtx, viewMtx, projMtx);  // the left wing
    _drawPlaneWing(false, modelMtx, viewMtx, projMtx); // the right wing
    _drawPlaneNose(modelMtx, viewMtx, projMtx);        // the nose
    _drawPlanePropeller(modelMtx, viewMtx, projMtx);   // the propeller
    _drawPlaneTail(modelMtx, viewMtx, projMtx);        // the tail
}

void Plane::flyForward() {
    //change x cord based on the cos of the direction
    _planeLocation.x += speed * cos(_direction);

    //check to make sure that the plane is inside the bounds
    if(_planeLocation.x > 55.0f){
        _planeLocation.x = 55.0f;
    } else if(_planeLocation.x < -55.0f){
        _planeLocation.x = -55.0f;
    }

    //update the z axis as well
    _planeLocation.z += speed * sin(_direction);

    //make sure the z axis is also within the bounds
    if(_planeLocation.z > 55.0f){
        _planeLocation.z = 55.0f;
    } else if(_planeLocation.z < -55.0f){
        _planeLocation.z = -55.0f;
    }

    //animate the propeller
    _propAngle += _propAngleRotationSpeed;
    if( _propAngle > 2.0f * M_PI ) _propAngle -= 2.0f * M_PI;
}

void Plane::flyBackward() {
    //change the x location
    _planeLocation.x -= speed * cos(_direction);

    //make sure that the plane stays in bounds when flying backwards
    if(_planeLocation.x > 55.0f){
        _planeLocation.x = 55.0f;
    } else if(_planeLocation.x < -55.0f){
        _planeLocation.x = -55.0f;
    }

    //change the z location
    _planeLocation.z -= speed * sin(_direction);

    //check inside the map
    if(_planeLocation.z > 55.0f){
        _planeLocation.z = 55.0f;
    } else if(_planeLocation.z < -55.0f){
        _planeLocation.z = -55.0f;
    }

    //animate the propellers
    _propAngle -= _propAngleRotationSpeed;
    if( _propAngle < 0.0f ) _propAngle += 2.0f * M_PI;
}

void Plane::_drawPlaneBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleBody );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidCube( 0.1 * 25 );
}

void Plane::_drawPlaneWing(bool isLeftWing, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleWing );

    if( isLeftWing ) {
        _drawPlaneGun(isLeftWing, modelMtx, viewMtx, projMtx);
        modelMtx = glm::rotate(modelMtx, -_rotateWingAngle, CSCI441::X_AXIS);

    } else {
        _drawPlaneGun(isLeftWing, modelMtx, viewMtx, projMtx);
        modelMtx = glm::rotate(modelMtx, _rotateWingAngle, CSCI441::X_AXIS);

    }

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorWing[0]);

    CSCI441::drawSolidCone( 0.05 * 25, 0.2 * 25, 16, 16 );
}
void Plane::_drawPlaneGun(bool isLeftWing, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    //HIERARCHICAL DRAWING AS GUN IS DRAWN WITHIN WING SINCE EVERY WING HAS A GUN!

    if (isLeftWing){
        modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.0f, -0.12f * 25));
        modelMtx = glm::rotate(modelMtx, _rotateWingAngle, CSCI441::Z_AXIS);
    }else{
        modelMtx = glm::translate( modelMtx, glm::vec3(-0.0f, 0.0f, 0.12f * 25) );
        modelMtx = glm::rotate(modelMtx, _rotateWingAngle, CSCI441::Z_AXIS);
    }

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorProp[0]);

    CSCI441::drawSolidCylinder( 0.05 * 25, 0.05 * 25, 0.05 * 25, 32, 32 );
}

void Plane::_drawPlaneNose(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::rotate( modelMtx, _rotateNoseAngle, CSCI441::Z_AXIS );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorNose[0]);

    CSCI441::drawSolidCone( 0.025 * 25, 0.3 * 25, 16, 16 );
}

void Plane::_drawPlanePropeller(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transProp );
    modelMtx1 = glm::rotate( modelMtx1, _propAngle, CSCI441::X_AXIS );
    modelMtx1 = glm::scale( modelMtx1, _scaleProp );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorProp[0]);

    CSCI441::drawSolidCube( 0.1 * 25);

    glm::mat4 modelMtx2 = glm::translate( modelMtx, _transProp );
    modelMtx2 = glm::rotate( modelMtx2, static_cast<GLfloat>(M_PI / 2.0f) + _propAngle, CSCI441::X_AXIS );
    modelMtx2 = glm::scale( modelMtx2, _scaleProp );

    _computeAndSendMatrixUniforms(modelMtx2, viewMtx, projMtx);

    CSCI441::drawSolidCube( 0.1 * 25);
}

void Plane::_drawPlaneTail(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorTail[0]);

    CSCI441::drawSolidCone( 0.02 * 25, 0.1 * 25, 16, 16 );
}

void Plane::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}
