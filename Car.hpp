//
// Created by Colby Schneider on 10/12/22.
//

#ifndef LAB05_CAR_HPP
#define LAB05_CAR_HPP
#include <CSCI441/OpenGLEngine.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Vehicle.h"

class Car: public Vehicle{
public:
    Car(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normMtx, GLint materialColorUniformLocation );

    void _drawCar(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);

    //movement
    void _moveForward(GLfloat WORLD_SIZE);
    void _moveBackward(GLfloat WORLD_SIZE);
    void _rotateRight();
    void _rotateLeft();

    GLfloat _getPositionX();
    GLfloat _getPositionY();

    //car state values
    GLfloat _positionX;
    GLfloat _positionZ;
    GLfloat _angle;

    //model transformation values
    GLfloat _wheelAngle;
    glm::vec3 _scaleBody;
    glm::vec3 _scaleBody2;
    glm::vec3 _translateBody;
    glm::vec3 _translateBody2;
    glm::vec3 _translateWheelsLR;
    glm::vec3 _translateWheelsUD;


private:
    GLuint _shaderProgramHandle;
    /// \desc stores the uniform locations needed for the plan information
    struct ShaderProgramUniformLocations {
        /// \desc location of the precomputed ModelViewProjection matrix
        GLint mvpMtx;
        /// \desc location of the precomputed Normal matrix
        GLint normalMtx;
        /// \desc location of the material diffuse color
        GLint materialColor;
    } _shaderProgramUniformLocations;

    //model colors
    glm::vec3 _wheelColor;
    glm::vec3 _bodyColor;

    //draw functions
    void _drawWheels(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawBody2(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
};


#endif //LAB05_CAR_HPP
