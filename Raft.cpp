//
// Created by Ben Schreiber on 10/9/22.
//

#include "Raft.h"


#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>


#include <glm/glm.hpp>
#include <CSCI441/Camera.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/objects.hpp>
#include <CSCI441/FreeCam.hpp>
#include <CSCI441/ShaderProgram.hpp>
#include <vector>

Raft::Raft(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint materialColorUniformLocation ){
    _angle = 0.0;
    _positionX = 0;
    _positionZ = 10;

    _leftPaddleAngle = 0;
    _rightPaddleAngle = 0;

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _logColor = glm::vec3(0.2,0,0);
    _oarColor = glm::vec3(0,0,0.2);

    _setupLogs();
    _setupOars();

}

void Raft::_drawRaft(glm::mat4 viewMtx, glm::mat4 projMtx) {
    glUseProgram(_shaderProgramHandle);


    // rotate and transform the raft according to current position and angle
    glm::mat4 transMtx = glm::translate(glm::mat4(1.0), glm::vec3(_positionX,0,_positionZ) );
    glm::mat4 rotateMtx = glm::rotate(glm::mat4(1.0), _angle, glm::vec3(0,1,0));

    // Draw the logs
    for(const LogData& currentLog : _logs){
        glm::mat4 logModelMtx = transMtx * rotateMtx * currentLog.modelMatrix;
        _computeAndSendMatrixUniforms(logModelMtx,viewMtx,projMtx);

        glUniform3fv(_shaderProgramUniformLocations.materialColor,1,&currentLog.color[0]);
        CSCI441::drawSolidCylinder(0.5,0.5,10.0,100,100);
    }

    // Rotate the oars in x direction according to current angle to paddle. Translate in z direction to get rowing effect
    glm::mat4 leftRotateX = glm::rotate(glm::mat4(1.0), (float) 0.2*sin(_leftPaddleAngle), glm::vec3(1,0,0));
    glm::mat4 leftTransZ = glm::translate(glm::mat4(1.0), glm::vec3(0,-0.5*cos(_leftPaddleAngle+M_PI/4),cos(_leftPaddleAngle)));
    glm::mat4 rightRotateX = glm::rotate(glm::mat4(1.0), (float) 0.2*sin(_rightPaddleAngle), glm::vec3(1,0,0));
    glm::mat4 rightTransZ = glm::translate(glm::mat4(1.0), glm::vec3(0,-0.5*cos(_rightPaddleAngle + M_PI/4),cos(_rightPaddleAngle)));


    // Draw right oar
    OarData rightOar = _oars[0];
    glm::mat4 rightOarModelMtx =  transMtx *  rotateMtx  * rightOar.modelMatrix * rightTransZ * rightRotateX;

    _computeAndSendMatrixUniforms(rightOarModelMtx,viewMtx,projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor,1,&rightOar.color[0]);
    CSCI441::drawSolidCylinder(0.2,0.2,5.0,50,50);

    // Draw left oar
    OarData leftOar = _oars[1];
    glm::mat4 leftOarModelMtx = transMtx  * rotateMtx  * leftOar.modelMatrix * leftTransZ * leftRotateX;

    _computeAndSendMatrixUniforms(leftOarModelMtx,viewMtx,projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor,1,&leftOar.color[0]);
    CSCI441::drawSolidCylinder(0.2,0.2,5.0,50,50);


}

void Raft::_setupLogs() {
    // generate 5 logs to make up the raft
    for(int i = 0; i < 5; i++){

        // Position them next to each other and above the water
        glm::mat4 positionMtx = glm::translate(glm::mat4(1.0), glm::vec3(i,0.5,0) );

        // rotate so they lie flat in the water
        glm::mat4 orientMtx = glm::rotate(glm::mat4(1.0), (float) M_PI/2, glm::vec3(1,0,0));

        // calculate model matrix
        glm:: mat4 modelMtx = positionMtx * orientMtx;

        // give them a color
        glm::vec3 color = glm::vec3(0.2,0.0,0.0);

        // store log properties
        LogData currentLog = {modelMtx, color};
        _logs.emplace_back(currentLog);
    }

}

void Raft::_setupOars() {

    // position left oar and compute model matrix
    glm::mat4 positionMtx = glm::translate(glm::mat4(1.0), glm::vec3(-0.5,2,5));
    glm::mat4 orientMtx = glm::rotate(glm::mat4(1.0), (float) (M_PI/2+0.41), glm::vec3(0,0,1));
    glm::mat4 modelMtx = positionMtx * orientMtx;

    // give oar a color
    glm::vec3 color = glm::vec3(0,0,0.2);

    // store left oar properties
    OarData leftLog = {modelMtx, color};
    _oars.emplace_back(leftLog);

    // do the same for right oar
    positionMtx = glm::translate(glm::mat4(1.0), glm::vec3(4.5,2,5));
    orientMtx = glm::rotate(glm::mat4(1.0), (float) (-M_PI/2-0.41), glm::vec3(0,0,1));
    modelMtx = positionMtx * orientMtx;

    OarData rightLog = {modelMtx, color};
    _oars.emplace_back(rightLog);
}


void Raft::_moveForward(GLfloat WORLD_SIZE){
    // If the raft isn't at the edge of the world, update it's x and z position according to the angle to move it forward
    if(_positionX + 0.1*sin(_angle)< WORLD_SIZE - 10 && _positionX + 0.1*sin(_angle)> -WORLD_SIZE + 10) {_positionX += 0.1*sin(_angle);}
    if(_positionZ + 0.1*cos(_angle)< WORLD_SIZE - 10 && _positionZ + 0.1*cos(_angle) > -WORLD_SIZE + 10) {_positionZ += 0.1*cos(_angle);}

}

void Raft::_moveBackward(GLfloat WORLD_SIZE){


    // If the raft isn't at the edge of the world, update it's x and z position according to the angle to move it backward
    if(_positionX - 0.1*sin(_angle)< WORLD_SIZE - 10 && _positionX - 0.1*sin(_angle)> -WORLD_SIZE + 10){_positionX -= 0.1*sin(_angle);}
    if(_positionZ - 0.1*cos(_angle)< WORLD_SIZE - 10 && _positionZ - 0.1*cos(_angle)> -WORLD_SIZE + 10){_positionZ -= 0.1*cos(_angle);}
}

void Raft::_rotateRight() {
    // increase the raft angle, pointing the raft right
    _angle -= 0.01;
}

void Raft::_rotateLeft(){
    // decrease the raft angle, pointing the raft left
    _angle += 0.01;
};

void Raft::_paddleForwardLeft() {
    // update the angle of the left oar, called when raft is moving forward or turning right
    _leftPaddleAngle += .1;
}

void Raft::_paddleForwardRight() {
    // update the angle of the right oar, called when raft in moving forward or turning left
    _rightPaddleAngle += .1;
}

void Raft::_paddleBackward() {
    // if moving backward, update both oars position
    _leftPaddleAngle -= .1;
    _rightPaddleAngle -= .1;
}

void Raft::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}

