//
// Created by Ben Schreiber on 10/9/22.
//

#ifndef LAB05_RAFT_HPP
#define LAB05_RAFT_HPP
#include <CSCI441/OpenGLEngine.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Vehicle.h"

class Raft: public Vehicle{
public:
    Raft(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normMtx, GLint materialColorUniformLocation );

    void _drawRaft(glm::mat4 viewMtx, glm::mat4 projMtx);


    //movement functions
    void _moveForward(GLfloat WORLD_SIZE);
    void _moveBackward(GLfloat WORLD_SIZE);
    void _rotateRight();
    void _rotateLeft();

    //movement animation functions
    void _paddleForwardRight();
    void _paddleForwardLeft();
    void _paddleBackward();


    GLfloat _getPositionX();
    GLfloat _getPositionY();

    //raft state values
    GLfloat _positionX;
    GLfloat _positionZ;
    GLfloat _angle;

private:
    //model transformation values
    GLfloat _leftPaddleAngle;
    GLfloat _rightPaddleAngle;

    //shader handle
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

    //model color vectors
    glm::vec3 _logColor;
    glm::vec3 _oarColor;

    //struct to hold log model data
    struct LogData {
        glm::mat4 modelMatrix;

        glm::vec3 color;
    };

    //struct to hold oar model data
    struct OarData {
        glm::mat4 modelMatrix;
        glm::vec3 color;
    };

    //vectors of all logs and oars
    std::vector<LogData> _logs;
    std::vector<OarData> _oars;

    //setup functions
    void _setupLogs();
    void _setupOars();

    //draw functions
    void _drawLogs( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawOars(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
};


#endif //LAB05_RAFT_HPP
