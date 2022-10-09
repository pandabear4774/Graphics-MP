#include "FreeCam.hpp"

#include <cmath>

void FreeCam::recomputeOrientation() {
    GLfloat _x = _radius*sin(_theta)*sin(_phi);
    GLfloat _y = -1*_radius*cos(_phi);
    GLfloat _z = -1*cos(_theta)*sin(_phi);
    _direction = glm::normalize(glm::vec3{_x, _y,_z});
    setLookAtPoint(_position+_direction);
    CSCI441::Camera::computeViewMatrix();

}

void FreeCam::moveForward(GLfloat movementFactor) {
    setPosition(_position + movementFactor*_direction);
    setLookAtPoint(_position+_direction);
    CSCI441::Camera::computeViewMatrix();
}

void FreeCam::moveBackward(GLfloat movementFactor) {
    setPosition(_position - movementFactor*_direction);
    setLookAtPoint(_position+_direction);
    CSCI441::Camera::computeViewMatrix();
}