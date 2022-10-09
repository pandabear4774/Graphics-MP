//
// Created by Ben Schreiber on 10/8/22.
//

#include "ArcBallCam.h"


#include <cmath>

void ArcBallCam::recomputeOrientation() {

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



void ArcBallCam::moveForward(GLfloat movementFactor) {
    // decrease the radius and recompute orientation and view matrix
    _radius -= movementFactor;
    recomputeOrientation();
}

void ArcBallCam::moveBackward(GLfloat movementFactor) {
    // increase the radius and recompute orientation and view matrix
    _radius += movementFactor;
    recomputeOrientation();
    }



