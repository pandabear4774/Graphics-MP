#include "FreeCam.hpp"

#include <cmath>

void FreeCam::recomputeOrientation() {
    //assign cartesian values
    GLfloat _x = _radius*sin(_theta)*sin(_phi);
    GLfloat _y = -1*_radius*cos(_phi);
    GLfloat _z = -1*cos(_theta)*sin(_phi);

    //update direction
    _direction = glm::normalize(glm::vec3{_x, _y,_z});
    //update lookAt point
    setLookAtPoint(_position+_direction);
    //compute view matrix
    CSCI441::Camera::computeViewMatrix();

}

void FreeCam::moveForward(GLfloat movementFactor) {
    //move location forward
    setPosition(_position + movementFactor*_direction);
    //move lookAt point forward
    setLookAtPoint(_position+_direction);
    //compute view matrix
    CSCI441::Camera::computeViewMatrix();
}

void FreeCam::moveBackward(GLfloat movementFactor) {
    //move location backward
    setPosition(_position - movementFactor*_direction);
    //move lookAt point backward
    setLookAtPoint(_position+_direction);
    //compute view matrix
    CSCI441::Camera::computeViewMatrix();
}