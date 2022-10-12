//
// Created by Kurtis Quant on 10/12/22.
//

#ifndef LAB05_FFPCAM_HPP
#define LAB05_FFPCAM_HPP

#include <CSCI441/Camera.hpp>

class FFPCam : public CSCI441::Camera {
public:
    void recomputeOrientation() final;

    float xL,yL,zL;

    void moveForward(GLfloat movementFactor) final;
    void moveBackward(GLfloat movementFactor) final;

};


#endif //LAB05_FFPCAM_HPP
