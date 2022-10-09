//
// Created by Ben Schreiber on 10/8/22.
//

#ifndef LAB05_ARCBALLCAM_H
#define LAB05_ARCBALLCAM_H

#endif //LAB05_ARCBALLCAM_H

#include <CSCI441/Camera.hpp>

class ArcBallCam : public CSCI441::Camera {
public:
    void recomputeOrientation() final;


    void moveForward(GLfloat movementFactor) final;
    void moveBackward(GLfloat movementFactor) final;

};
