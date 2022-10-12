//
// Created by Kurtis Quant on 10/12/22.
//

#include "FFPCam.hpp"

void FFPCam::recomputeOrientation() {

    // update camera position based on the radius, theta and phi, and the look at point

    GLfloat x = _radius*sin(_theta)*sin(_phi) + _lookAtPoint.x;
    GLfloat y = -_radius*cos(_phi) + _lookAtPoint.y;
    GLfloat z = -_radius*cos(_theta)*sin(_phi) + _lookAtPoint.z;

    _position = glm::vec3(x,y,z);

    // compute the direction vector based on the position and look at point
    _direction = normalize(_position-_lookAtPoint);


    // compute the view matrix
    CSCI441::Camera::computeViewMatrix();

}



void FFPCam::moveForward(GLfloat movementFactor) {
    // decrease the radius and recompute orientation and view matrix
    _radius -= movementFactor;
    recomputeOrientation();
}

void FFPCam::moveBackward(GLfloat movementFactor) {
    // increase the radius and recompute orientation and view matrix
    _radius += movementFactor;
    recomputeOrientation();
}
