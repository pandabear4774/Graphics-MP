//
// Created by Colby Schneider on 10/12/22.
//

#include "Car.hpp"


#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>


#include <glm/glm.hpp>


Car::Car(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normMtx, GLint materialColorUniformLocation ){
    _angle = 0.0;

    _positionX = 0;
    _positionZ = 10;
    _location = {_positionX,0,_positionZ};

    _wheelAngle = 0;

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;
}

void Car::_drawCar(glm::mat4 viewMtx, glm::mat4 projMtx) {
    glUseProgram(_shaderProgramHandle);


    // rotate and transform the Car according to current position and angle
    glm::mat4 transMtx = glm::translate(glm::mat4(1.0), glm::vec3(_positionX,0,_positionZ) );
    glm::mat4 rotateMtx = glm::rotate(glm::mat4(1.0), _angle, glm::vec3(0,1,0));



}

void Car::_setupWheels() {
    // generate 5 logs to make up the Car
    for(int i = 0; i < 4; i++){

    }

}

void Car::_drawBody(glm::mat4 viewMtx, glm::mat4 projMtx){

}

void Car::_drawWheels(glm::mat4 viewMtx, glm::mat4 projMtx){

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
}

void Car::_rotateLeft(){
    // decrease the Car angle, pointing the Car left
    _angle += 0.01;
};

void Car::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}

