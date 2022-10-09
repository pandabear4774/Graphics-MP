#ifndef LAB02_FREE_CAM_HPP
#define LAB02_FREE_CAM_HPP

#include <CSCI441/Camera.hpp>

class FreeCam : public CSCI441::Camera {
public:
    void recomputeOrientation() final;

    void moveForward(GLfloat movementFactor) final;
    void moveBackward(GLfloat movementFactor) final;
};

#endif // LAB02_FREE_CAM_HPP