//
// Created by Colby Schneider on 10/12/22.
//

#include "Car.hpp"


#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>


#include <glm/glm.hpp>

#include <iostream>


Car::Car(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normMtx, GLint materialColorUniformLocation ){
    //// CAR STATE VALUES
    _angle = 0.0;
    _positionX = 10;
    _positionZ = -10;
    _location = {_positionX,0,_positionZ};

    ////ASSIGN UNIFORM LOCATIONS
    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    //// MODEL TRANSFORMATION VALUES
    _scaleBody = glm::vec3(2.0f, 0.5f, 4.0f);
    _scaleBody2 = glm::vec3(2.0f, 0.75f, 2.0f);
    _wheelAngle = 0;
    _translateBody = glm::vec3(0.0f, 0.75f, 0.0f);
    _translateBody2 = glm::vec3(0.0f, 1.35f, -0.5f);
    _translateWheelsLR = glm::vec3(1.0f, 0.0f, 0.0f);
    _translateWheelsUD = glm::vec3(0.0f, 0.0f, 1.25f);
    _bodyColor = glm::vec3(1, 0, 0);
    _wheelColor = glm::vec3(0, 0, 0);
}

void Car::_drawCar(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    glUseProgram(_shaderProgramHandle);


    // rotate and transform the Car according to current position and angle
    modelMtx = glm::translate(modelMtx, glm::vec3(_positionX,0,_positionZ) );
    modelMtx = glm::rotate(modelMtx, _angle, glm::vec3(0,1,0));

    //draw model components
    _drawBody(modelMtx, viewMtx, projMtx);
    _drawBody2(modelMtx, viewMtx, projMtx);
    _drawWheels(modelMtx, viewMtx, projMtx);


}

void Car::_drawWheels(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const{
    ////FRONT RIGHT WHEEL
    //translate to correct position
    glm::mat4 modelMtxFR = glm::translate(modelMtx, _translateWheelsUD + _translateWheelsLR + glm::vec3(0, 0.3f, 0));
    //rotate to steering angle
    modelMtxFR = glm::rotate(modelMtxFR, (float)_wheelAngle, glm::vec3(0, 1, 0));
    //rotate onto its side
    modelMtxFR = glm::rotate(modelMtxFR, (float) M_PI/2, glm::vec3(0, 0, 1));
    //prepare to draw
    _computeAndSendMatrixUniforms(modelMtxFR,viewMtx,projMtx);
    glUniform3fv(_shaderProgramUniformLocations.materialColor,1,&_wheelColor[0]);
    //draw wheel
    CSCI441::drawSolidCylinder(0.25,0.25,0.2,50,50);

    ////FRONT LEFT WHEEL
    //translate to correct position
    glm::mat4 modelMtxFL = glm::translate(modelMtx, _translateWheelsUD - _translateWheelsLR + glm::vec3(0.2f, 0.3f, 0));
    //rotate to steering angle
    modelMtxFL = glm::rotate(modelMtxFL, (float)_wheelAngle, glm::vec3(0, 1, 0));
    //rotate onto its side
    modelMtxFL = glm::rotate(modelMtxFL, (float) M_PI/2, glm::vec3(0, 0, 1));
    //prepare to draw
    _computeAndSendMatrixUniforms(modelMtxFL,viewMtx,projMtx);
    glUniform3fv(_shaderProgramUniformLocations.materialColor,1,&_wheelColor[0]);
    //draw wheel
    CSCI441::drawSolidCylinder(0.25,0.25,0.2,50,50);

    ////BACK RIGHT WHEEL
    //translate to correct position
    glm::mat4 modelMtxBR = glm::translate(modelMtx, -_translateWheelsUD + _translateWheelsLR + glm::vec3(0, 0.3f, 0));
    //rotate onto its side
    modelMtxBR = glm::rotate(modelMtxBR, (float) M_PI/2, glm::vec3(0, 0, 1));
    //prepare to draw
    _computeAndSendMatrixUniforms(modelMtxBR,viewMtx,projMtx);
    glUniform3fv(_shaderProgramUniformLocations.materialColor,1,&_wheelColor[0]);
    //draw wheel
    CSCI441::drawSolidCylinder(0.25,0.25,0.2,50,50);

    ////BACK LEFT WHEEL
    //translate to correct position
    glm::mat4 modelMtxBL = glm::translate(modelMtx, -_translateWheelsUD - _translateWheelsLR + glm::vec3(0.2f, 0.3f, 0));
    //rotate onto its side
    modelMtxBL = glm::rotate(modelMtxBL, (float) M_PI/2, glm::vec3(0, 0, 1));
    //prepare to draw
    _computeAndSendMatrixUniforms(modelMtxBL,viewMtx,projMtx);
    glUniform3fv(_shaderProgramUniformLocations.materialColor,1,&_wheelColor[0]);
    //draw wheel
    CSCI441::drawSolidCylinder(0.25,0.25,0.2,50,50);
}

void Car::_drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const{
    ////DRAW LOWER BODY
    //translate to correct position
    modelMtx = glm::translate(modelMtx, _translateBody);
    //resize
    modelMtx = glm::scale( modelMtx, _scaleBody );
    //prepare to draw
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_bodyColor[0]);
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    //draw lower body
    CSCI441::drawSolidCube(1);
}

void Car::_drawBody2(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const{
    ////DRAW UPPER BODY
    //translate to correct position
    modelMtx = glm::translate(modelMtx, _translateBody2);
    //resize
    modelMtx = glm::scale( modelMtx, _scaleBody2 );
    //prepare to draw
    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_bodyColor[0]);
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    //draw upper body
    CSCI441::drawSolidCube(1);
}

void Car::_moveForward(GLfloat WORLD_SIZE){
    // If the Car isn't at the edge of the world, update it's x and z position according to the angle to move it forward
    if(_positionX + 0.1*sin(_angle)< WORLD_SIZE - 10 && _positionX + 0.1*sin(_angle)> -WORLD_SIZE + 10) {_positionX += 0.1*sin(_angle);}
    if(_positionZ + 0.1*cos(_angle)< WORLD_SIZE - 10 && _positionZ + 0.1*cos(_angle) > -WORLD_SIZE + 10) {_positionZ += 0.1*cos(_angle);}
    _location = {_positionX, 0, _positionZ};
}

void Car::_moveBackward(GLfloat WORLD_SIZE){
    // If the Car isn't at the edge of the world, update it's x and z position according to the angle to move it backward
    if(_positionX - 0.1*sin(_angle)< WORLD_SIZE - 10 && _positionX - 0.1*sin(_angle)> -WORLD_SIZE + 10){_positionX -= 0.1*sin(_angle);}
    if(_positionZ - 0.1*cos(_angle)< WORLD_SIZE - 10 && _positionZ - 0.1*cos(_angle)> -WORLD_SIZE + 10){_positionZ -= 0.1*cos(_angle);}
    _location = {_positionX, 0, _positionZ};
}

void Car::_rotateRight() {
    // increase the Car angle, pointing the Car right
    _angle -= 0.01;
    //increase wheel angle, pointing front wheels right
    if (_wheelAngle > -(float)M_PI/4){
        _wheelAngle -= (float)M_PI/32;
    }
}

void Car::_rotateLeft(){
    // decrease the Car angle, pointing the Car left
    _angle += 0.01;
    //decrease wheel angle, pointing front wheels left
    if (_wheelAngle < (float)M_PI/4){
        _wheelAngle += (float)M_PI/32;
    }
};

void Car::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}

