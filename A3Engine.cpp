#include "A3Engine.hpp"
#include "ArcBallCam.h"

#include <CSCI441/objects.hpp>
#include <CSCI441/FreeCam.hpp>

//*************************************************************************************
//
// Helper Functions

#ifndef M_PI
#define M_PI 3.14159265

#endif

/// \desc Simple helper function to return a random number between 0.0f and 1.0f.
GLfloat getRand() {
    return (GLfloat)rand() / (GLfloat)RAND_MAX;
}

//*************************************************************************************
//
// Public Interface

A3Engine::A3Engine()
         : CSCI441::OpenGLEngine(4, 1,
                                 640, 480,
                                 "Lab05: Flight Simulator v0.41 alpha") {

    for(auto& _key : _keys) _key = GL_FALSE;

    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;

    _raftAngle = 0.0;
    _raftPositionX = 0;
    _raftPositionZ = 10;

    _leftPaddleAngle = 0;
    _rightPaddleAngle = 0;

}

A3Engine::~A3Engine() {
    delete _arcBallCam;
}

void A3Engine::handleKeyEvent(GLint key, GLint action) {
    if(key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if(action == GLFW_PRESS) {
        switch( key ) {
            // quit!
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                setWindowShouldClose();
                break;

            default: break; // suppress CLion warning
        }
    }
}

void A3Engine::handleMouseButtonEvent(GLint button, GLint action) {
    // if the event is for the left mouse button
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void A3Engine::handleCursorPositionEvent(glm::vec2 currMousePosition) {
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if(_mousePosition.x == MOUSE_UNINITIALIZED) {
        _mousePosition = currMousePosition;
    }

    // if the left mouse button is being held down while the mouse is moving
    if(_leftMouseButtonState == GLFW_PRESS) {
        // rotate the camera by the distance the mouse moved

        _arcBallCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                         (_mousePosition.y - currMousePosition.y) * 0.005f );

        _arcBallCam->recomputeOrientation();


    }

    // update the mouse position
    _mousePosition = currMousePosition;
}

//*************************************************************************************
//
// Engine Setup

void A3Engine::_setupGLFW() {
    CSCI441::OpenGLEngine::_setupGLFW();

    // set our callbacks
    glfwSetKeyCallback(_window, lab05_engine_keyboard_callback);
    glfwSetMouseButtonCallback(_window, lab05_engine_mouse_button_callback);
    glfwSetCursorPosCallback(_window, lab05_engine_cursor_callback);
}

void A3Engine::_setupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                        // enable depth testing
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	    // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	        // clear the frame buffer to black
}

void A3Engine::_setupShaders() {
    _lightingShaderProgram = new CSCI441::ShaderProgram("shaders/lab05.v.glsl", "shaders/lab05.f.glsl" );
    _lightingShaderUniformLocations.mvpMatrix      = _lightingShaderProgram->getUniformLocation("mvpMatrix");
    _lightingShaderUniformLocations.materialColor  = _lightingShaderProgram->getUniformLocation("materialColor");

    _lightingShaderAttributeLocations.vPos         = _lightingShaderProgram->getAttributeLocation("vPos");

}

void A3Engine::_setupBuffers() {
    CSCI441::setVertexAttributeLocations( _lightingShaderAttributeLocations.vPos );

    _createGroundBuffers();
    _generateEnvironment();
    _setupLogs();
    _setupOars();
}

void A3Engine::_createGroundBuffers() {
    struct Vertex {
        GLfloat x, y, z;

    };

    Vertex groundQuad[4] = {
            {-1.0f, 0.0f, -1.0f},
            { 1.0f, 0.0f, -1.0f},
            {-1.0f, 0.0f,  1.0f},
            { 1.0f, 0.0f,  1.0f}
    };

    GLushort indices[4] = {0,1,2,3};

    _numGroundPoints = 4;

    glGenVertexArrays(1, &_groundVAO);
    glBindVertexArray(_groundVAO);

    GLuint vbods[2];       // 0 - VBO, 1 - IBO
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundQuad), groundQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vPos);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);



    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void A3Engine::_generateEnvironment() {
    //******************************************************************
    // parameters to make up our grid size and spacing, feel free to
    // play around with this
    const GLfloat GRID_WIDTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_LENGTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;
    //******************************************************************

    srand( time(0) );                                                   // seed our RNG

    // psych! everything's on a grid.

    // Draws "buoys" instead of buildings
    for(int i = LEFT_END_POINT; i < RIGHT_END_POINT; i += GRID_SPACING_WIDTH) {
        for(int j = BOTTOM_END_POINT; j < TOP_END_POINT; j += GRID_SPACING_LENGTH) {

            if( i % 2 && j % 2 && getRand() < 0.025f ) {
                // translate to spot
                glm::mat4 transToSpotMtx = glm::translate( glm::mat4(1.0), glm::vec3(i, 0.0f, j) );

                // compute random radius
                GLdouble radius = powf(getRand(), 2)*5 + 1;
                // scale to buoy size
                glm::mat4 scaleToHeightMtx = glm::scale( glm::mat4(1.0), glm::vec3(radius, radius, radius) );

                // translate up to grid

                // compute full model matrix
                glm::mat4 modelMatrix = transToSpotMtx * scaleToHeightMtx;

                // compute random color
                glm::vec3 color( getRand(), getRand(), getRand() );
                // store buoy properties
                BuoyData currentBuoy = {modelMatrix, color};
                _buoys.emplace_back( currentBuoy );
            }
        }
    }
}

void A3Engine::_setupLogs() {
    // generate 5 logs to make up the raft
    for(int i = 0; i < 5; i++){

        // Position them next to each other and above the water
        glm::mat4 positionMtx = glm::translate(glm::mat4(1.0), glm::vec3(i,0.5,0) );

        // rotate so they lie flat in the water
        glm::mat4 orientMtx = glm::rotate(glm::mat4(1.0), (float) M_PI/2, glm::vec3(1,0,0));

        // calculate model matrix
        glm:: mat4 modelMtx = positionMtx * orientMtx;

        // give them a color
        glm::vec3 color = glm::vec3(0.2,0.0,0.0);

        // store log properties
        LogData currentLog = {modelMtx, color};
        _logs.emplace_back(currentLog);
    }

}

void A3Engine::_setupOars() {

    // position left oar and compute model matrix
    glm::mat4 positionMtx = glm::translate(glm::mat4(1.0), glm::vec3(-0.5,2,5));
    glm::mat4 orientMtx = glm::rotate(glm::mat4(1.0), (float) (M_PI/2+0.41), glm::vec3(0,0,1));
    glm::mat4 modelMtx = positionMtx * orientMtx;

    // give oar a color
    glm::vec3 color = glm::vec3(0,0,0.2);

    // store left oar properties
    OarData leftLog = {modelMtx, color};
    _oars.emplace_back(leftLog);

    // do the same for right oar
    positionMtx = glm::translate(glm::mat4(1.0), glm::vec3(4.5,2,5));
    orientMtx = glm::rotate(glm::mat4(1.0), (float) (-M_PI/2-0.41), glm::vec3(0,0,1));
    modelMtx = positionMtx * orientMtx;

    OarData rightLog = {modelMtx, color};
    _oars.emplace_back(rightLog);
}

void A3Engine::_moveForward(){
    // If the raft isn't at the edge of the world, update it's x and z position according to the angle to move it forward
    if(_raftPositionX + 0.1*sin(_raftAngle)< WORLD_SIZE - 10 && _raftPositionX + 0.1*sin(_raftAngle)> -WORLD_SIZE + 10) {_raftPositionX += 0.1*sin(_raftAngle);}
    if(_raftPositionZ + 0.1*cos(_raftAngle)< WORLD_SIZE - 10 && _raftPositionZ + 0.1*cos(_raftAngle) > -WORLD_SIZE + 10) {_raftPositionZ += 0.1*cos(_raftAngle);}

}

void A3Engine::_moveBackward(){
    // If the raft isn't at the edge of the world, update it's x and z position according to the angle to move it backward
    if(_raftPositionX - 0.1*sin(_raftAngle)< WORLD_SIZE - 10 && _raftPositionX - 0.1*sin(_raftAngle)> -WORLD_SIZE + 10){_raftPositionX -= 0.1*sin(_raftAngle);}
    if(_raftPositionZ - 0.1*cos(_raftAngle)< WORLD_SIZE - 10 && _raftPositionZ - 0.1*cos(_raftAngle)> -WORLD_SIZE + 10){_raftPositionZ -= 0.1*cos(_raftAngle);}
}

void A3Engine::_rotateRight() {
    // increase the raft angle, pointing the raft right
    _raftAngle -= 0.01;
}

void A3Engine::_rotateLeft(){
    // decrease the raft angle, pointing the raft left
    _raftAngle += 0.01;
};

void A3Engine::_paddleForwardLeft() {
    // update the angle of the left oar, called when raft is moving forward or turning right
    _leftPaddleAngle += .1;
}

void A3Engine::_paddleForwardRight() {
    // update the angle of the right oar, called when raft in moving forward or turning left
    _rightPaddleAngle += .1;
}

void A3Engine::_paddleBackward() {
    // if moving backward, update both oars position
    _leftPaddleAngle -= .1;
    _rightPaddleAngle -= .1;
}

void A3Engine::_setupScene() {
    // Create new arcball cam
    _arcBallCam = new ArcBallCam();

    // set the lookout point to be at the raft
    _arcBallCam->setLookAtPoint(glm::vec3(_raftPositionX,0.0 , _raftPositionZ));

    // set initial radius and angles
    _arcBallCam->setRadius(100);
    _arcBallCam->setTheta( 3);
    _arcBallCam->setPhi(2);
    _arcBallCam->recomputeOrientation();
    _cameraSpeed = glm::vec2(0.25f, 0.02f);


}

//*************************************************************************************
//
// Engine Cleanup

void A3Engine::_cleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _lightingShaderProgram;
}

void A3Engine::_cleanupBuffers() {
    fprintf( stdout, "[INFO]: ...deleting VAOs....\n" );
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays( 1, &_groundVAO );

    fprintf( stdout, "[INFO]: ...deleting VBOs....\n" );
    CSCI441::deleteObjectVBOs();

    fprintf( stdout, "[INFO]: ...deleting models..\n" );
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void A3Engine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // use our lighting shader program
    _lightingShaderProgram->useProgram();

    //// BEGIN DRAWING THE GROUND PLANE ////
    // draw the ground plane (water)
    glm::mat4 groundModelMtx = glm::scale( glm::mat4(1.0f), glm::vec3(WORLD_SIZE, 1.0f, WORLD_SIZE));
    _computeAndSendMatrixUniforms(groundModelMtx, viewMtx, projMtx);

    glm::vec3 groundColor(0.0f, 0.2f, 0.3f);
    glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &groundColor[0]);

    glBindVertexArray(_groundVAO);
    glDrawElements(GL_TRIANGLE_STRIP, _numGroundPoints, GL_UNSIGNED_SHORT, (void*)0);
    //// END DRAWING THE GROUND PLANE ////

    //// BEGIN DRAWING THE BUOYS ////

    for( const BuoyData& currentBuoy : _buoys ) {
        _computeAndSendMatrixUniforms(currentBuoy.modelMatrix, viewMtx, projMtx);

        glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &currentBuoy.color[0]);

        CSCI441::drawSolidSphere(1.0, 100,100);
        
    }
    //// END DRAWING THE BUOYS ////

    // rotate and transform the raft according to current position and angle
    glm::mat4 transMtx = glm::translate(glm::mat4(1.0), glm::vec3(_raftPositionX,0,_raftPositionZ) );
    glm::mat4 rotateMtx = glm::rotate(glm::mat4(1.0), _raftAngle, glm::vec3(0,1,0));

    // Draw the logs
    for(const LogData& currentLog : _logs){
        glm::mat4 logModelMtx = transMtx * rotateMtx * currentLog.modelMatrix;
        _computeAndSendMatrixUniforms(logModelMtx,viewMtx,projMtx);

        glUniform3fv(_lightingShaderUniformLocations.materialColor,1,&currentLog.color[0]);
        CSCI441::drawSolidCylinder(0.5,0.5,10.0,100,100);
    }

    // Rotate the oars in x direction according to current angle to paddle. Translate in z direction to get rowing effect
    glm::mat4 leftRotateX = glm::rotate(glm::mat4(1.0), (float) 0.2*sin(_leftPaddleAngle), glm::vec3(1,0,0));
    glm::mat4 leftTransZ = glm::translate(glm::mat4(1.0), glm::vec3(0,-0.5*cos(_leftPaddleAngle+M_PI/4),cos(_leftPaddleAngle)));
    glm::mat4 rightRotateX = glm::rotate(glm::mat4(1.0), (float) 0.2*sin(_rightPaddleAngle), glm::vec3(1,0,0));
    glm::mat4 rightTransZ = glm::translate(glm::mat4(1.0), glm::vec3(0,-0.5*cos(_rightPaddleAngle + M_PI/4),cos(_rightPaddleAngle)));


    // Draw right oar
    OarData rightOar = _oars[0];
    glm::mat4 rightOarModelMtx =  transMtx *  rotateMtx  * rightOar.modelMatrix * rightTransZ * rightRotateX;

    _computeAndSendMatrixUniforms(rightOarModelMtx,viewMtx,projMtx);

    glUniform3fv(_lightingShaderUniformLocations.materialColor,1,&rightOar.color[0]);
    CSCI441::drawSolidCylinder(0.2,0.2,5.0,50,50);

    // Draw left oar
    OarData leftOar = _oars[1];
    glm::mat4 leftOarModelMtx = transMtx  * rotateMtx  * leftOar.modelMatrix * leftTransZ * leftRotateX;

    _computeAndSendMatrixUniforms(leftOarModelMtx,viewMtx,projMtx);

    glUniform3fv(_lightingShaderUniformLocations.materialColor,1,&leftOar.color[0]);
    CSCI441::drawSolidCylinder(0.2,0.2,5.0,50,50);

}

void A3Engine::_updateScene() {
    // fly
    if( _keys[GLFW_KEY_SPACE] ) {
        // go backward if shift held down
        if( _keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT] ) {
            _arcBallCam->moveBackward(_cameraSpeed.x);
        }
        // go forward
        else {
            _arcBallCam->moveForward(_cameraSpeed.x);
        }
    }
    // turn right
    if( _keys[GLFW_KEY_D] ) {
        _rotateRight();
        _paddleForwardLeft();
    }
    // turn left
    if( _keys[GLFW_KEY_A] ) {
        _rotateLeft();
        _paddleForwardRight();
    }
    // go forward
    if( _keys[GLFW_KEY_W] ) {
        _moveForward();
        _paddleForwardLeft();
        _paddleForwardRight();

        // update lookatPoint and recompute orientation
        _arcBallCam->setLookAtPoint(glm::vec3(_raftPositionX, 0, _raftPositionZ));
        _arcBallCam->recomputeOrientation();
    }
    // go backward
    if( _keys[GLFW_KEY_S] ) {
        _moveBackward();
        _paddleBackward();

        // update lookout point and recomputer orientation
        _arcBallCam->setLookAtPoint(glm::vec3(_raftPositionX, 0, _raftPositionZ));
        _arcBallCam->recomputeOrientation();
    }
}

void A3Engine::run() {
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(_window) ) {	        // check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				        // work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( _window, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // set the projection matrix based on the window size
        // use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        glm::mat4 projectionMatrix = glm::perspective( 45.0f, (GLfloat) framebufferWidth / (GLfloat) framebufferHeight, 0.001f, 1000.0f );

        // set up our look at matrix to position our camera
        glm::mat4 viewMatrix = _arcBallCam->getViewMatrix();

        // draw everything to the window
        _renderScene(viewMatrix, projectionMatrix);

        _updateScene();

        glfwSwapBuffers(_window);                       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                // check for any events and signal to redraw screen
    }
}

//*************************************************************************************
//
// Private Helper FUnctions

void A3Engine::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mvpMatrix, mvpMtx);


}

//*************************************************************************************
//
// Callbacks

void lab05_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);

    // pass the key and action through to the engine
    engine->handleKeyEvent(key, action);
}

void lab05_engine_cursor_callback(GLFWwindow *window, double x, double y ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);

    // pass the cursor position through to the engine
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

void lab05_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent(button, action);
}
