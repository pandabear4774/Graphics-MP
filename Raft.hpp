//
// Created by Ben Schreiber on 10/9/22.
//

#ifndef LAB05_RAFT_HPP
#define LAB05_RAFT_HPP
#include <CSCI441/OpenGLEngine.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Raft {
public:
    Raft(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normMtx, GLint materialColorUniformLocation );

    void _drawRaft(glm::mat4 viewMtx, glm::mat4 projMtx);



    void _moveForward(GLfloat WORLD_SIZE);

    void _moveBackward(GLfloat WORLD_SIZE);

    void _rotateRight();

    void _rotateLeft();

    void _paddleForwardRight();

    void _paddleForwardLeft();

    void _paddleBackward();

    GLfloat _getPositionX();

    GLfloat _getPositionY();

    GLfloat _positionX;

    GLfloat _positionZ;

    GLfloat _angle;

private:




    GLfloat _leftPaddleAngle;

    GLfloat _rightPaddleAngle;

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

    glm::vec3 _logColor;

    glm::vec3 _oarColor;

    struct LogData {
        glm::mat4 modelMatrix;

        glm::vec3 color;
    };

    struct OarData {
        glm::mat4 modelMatrix;
        glm::vec3 color;
    };

    std::vector<LogData> _logs;

    std::vector<OarData> _oars;

    void _setupLogs();

    void _setupOars();

    void _drawLogs( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _drawOars(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
};


#endif //LAB05_RAFT_HPP
