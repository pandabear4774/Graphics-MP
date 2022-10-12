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

}

A3Engine::~A3Engine() {
    delete _camera;
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

        if(cameraType == 1 || cameraType == 2) {
            _camera->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                            (_mousePosition.y - currMousePosition.y) * 0.005f );

            _camera->recomputeOrientation();
        } else if(cameraType == 3){
            _camera->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                            (_mousePosition.y - currMousePosition.y) * 0.005f );

            _camera->recomputeOrientation();
        }



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
    // TODO #3A: assign uniforms
    _lightingShaderUniformLocations.normMtx        = _lightingShaderProgram->getUniformLocation("normMatrix");
    _lightingShaderUniformLocations.lightDirection = _lightingShaderProgram->getUniformLocation("lightDir");
    _lightingShaderUniformLocations.lightColor     = _lightingShaderProgram->getUniformLocation("lightColor");

    _lightingShaderAttributeLocations.vPos         = _lightingShaderProgram->getAttributeLocation("vPos");
    // TODO #3B: assign attributes
    _lightingShaderAttributeLocations.vecNormal    = _lightingShaderProgram->getAttributeLocation("vecNormal");

}

void A3Engine::_setupBuffers() {
    CSCI441::setVertexAttributeLocations( _lightingShaderAttributeLocations.vPos,  _lightingShaderAttributeLocations.vecNormal);

    _raft = new Raft(_lightingShaderProgram->getShaderProgramHandle(),
                     _lightingShaderUniformLocations.mvpMatrix,
                     _lightingShaderUniformLocations.normMtx,
                     _lightingShaderUniformLocations.materialColor);

    _plane = new Plane(_lightingShaderProgram->getShaderProgramHandle(),
                       _lightingShaderUniformLocations.mvpMatrix,
                       _lightingShaderUniformLocations.normMtx,
                       _lightingShaderUniformLocations.materialColor);

    _currVehicle = _raft;

    _createGroundBuffers();
    _generateEnvironment();

}

void A3Engine::_createGroundBuffers() {
    struct Vertex {
        GLfloat x, y, z;
        GLfloat normalX, normalY, normalZ;

    };

    Vertex groundQuad[4] = {
            {-1.0f, 0.0f, -1.0f,-1.0,1.0,-1.0},
            { 1.0f, 0.0f, -1.0f,1.0,1.0,-1.0},
            {-1.0f, 0.0f,  1.0f,-1.0,1.0,1.0},
            { 1.0f, 0.0f,  1.0f,1.0,1.0,1.0}
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

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vecNormal);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vecNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));

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

void A3Engine::_setupScene() {
    _changeToFreeCam();
    _cameraSpeed = glm::vec2(0.25f, 0.02f);


    glm::vec3 lightDirection = glm::vec3(-1,-1,-1);
    glm::vec3 lightColor = glm::vec3(1,1,1);

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightDirection,1,&lightDirection[0]);

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightColor,1,&lightColor[0]);

}
void A3Engine::_changeToFFPCam() {
    _camera = new FFPCam();

    _camera->setPosition(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y +1, _currVehicle->_location.z));

    // set initial radius and angles
    _camera->setPhi(2);
    _camera->setTheta(_currVehicle->_direction);
    _camera->recomputeOrientation();

}
void A3Engine::_changeToFreeCam() {
    _camera = new CSCI441::FreeCam();
    _camera->setPosition( glm::vec3(1.5*WORLD_SIZE, 40.0f, WORLD_SIZE) );
    _camera->setTheta( -M_PI / 3.0f );
    _camera->setPhi( _currVehicle->_direction );
    _camera->recomputeOrientation();
}

void A3Engine::_changeToArcBallCam() {

    // Create new arcball cam
    _camera = new ArcBallCam();

    // set the lookout point to be at the raft
    _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y , _currVehicle->_location.z));

    // set initial radius and angles
    _camera->setRadius(30);
    _camera->setTheta( 1);
    _camera->setPhi(2);
    _camera->recomputeOrientation();
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

    glm::vec3 groundColor(0.52f, 0.77f, 0.91f);
    glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &groundColor[0]);

    glBindVertexArray(_groundVAO);
    glDrawElements(GL_TRIANGLE_STRIP, _numGroundPoints, GL_UNSIGNED_SHORT, (void*)0);
    //// END DRAWING THE GROUND PLANE ////

    //DRAWING THE PLANE
    glm::mat4 modelMtx(1.0f);
    // we are going to cheat and use our look at point to place our plane so that it is always in view
    modelMtx = glm::translate( modelMtx, _plane->_location );
    // rotate the plane with our camera theta direction (we need to rotate the opposite direction so that we always look at the back)
    float pie2 = M_PI / 2.0f;

    modelMtx = glm::rotate( modelMtx, -_plane->_direction - pie2, CSCI441::Y_AXIS );
    // rotate the plane with our camera phi direction
    modelMtx = glm::rotate( modelMtx,  pie2, CSCI441::X_AXIS );
    // draw our plane now
    _plane->drawPlane( modelMtx, viewMtx, projMtx );


    //// BEGIN DRAWING THE BUOYS ////

    for( const BuoyData& currentBuoy : _buoys ) {
        _computeAndSendMatrixUniforms(currentBuoy.modelMatrix, viewMtx, projMtx);

        glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &currentBuoy.color[0]);

        CSCI441::drawSolidSphere(1.0, 100,100);

    }
    //// END DRAWING THE BUOYS ////

    _raft->_drawRaft(viewMtx, projMtx);
}

void A3Engine::_updateScene() {
    // fly
    if( _keys[GLFW_KEY_SPACE] ) {
        // go backward if shift held down
        if( _keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT] ) {
            _camera->moveBackward(_cameraSpeed.x);
        }
            // go forward
        else {
            _camera->moveForward(_cameraSpeed.x);
        }
    }
    // turn right
    if( _keys[GLFW_KEY_D] ) {
        if (_currVehicle == _raft) {
            _raft->_rotateRight();
            _raft->_paddleForwardLeft();

            _currVehicle->_direction = -_raft->_angle;

            if(cameraType == 3){
                _camera->setTheta(_currVehicle->_direction);
                _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y + 5 , _currVehicle->_location.z));
                _camera->recomputeOrientation();
            }
        }


        if (_currVehicle == _plane) {


            _currVehicle->_direction += _plane->speed / 5.0f;
            _plane->_direction += _plane->speed / 5.0f;
            if (_plane->_direction > M_PI * 2.0f) {
                _plane->_direction -= M_PI * 2.0f;
            }
            if(cameraType == 3){
                _camera->setTheta(_currVehicle->_direction - M_PI/2);
                _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y + 5 , _currVehicle->_location.z));
                _camera->recomputeOrientation();
            }
        }
    }
    // turn left
    if( _keys[GLFW_KEY_A] ) {
        if(_currVehicle == _raft) {
            _raft->_rotateLeft();
            _raft->_paddleForwardRight();

            _currVehicle->_direction = -_raft->_angle;

            if(cameraType == 3){
                _camera->setTheta(_currVehicle->_direction);
                _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y + 5 , _currVehicle->_location.z));
                _camera->recomputeOrientation();
            }
        }

        if(_currVehicle == _plane) {

            _currVehicle->_direction -= _plane->speed / 5.0f;
            _plane->_direction -= _plane->speed / 5.0f;
            if (_plane->_direction < 0.0f) {
                _plane->_direction += M_PI * 2.0f;
            }
            if(cameraType == 3){
                _camera->setTheta(_currVehicle->_direction - M_PI/2);
                _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y + 5 , _currVehicle->_location.z));
                _camera->recomputeOrientation();
            }
        }
    }
    // go forward
    if( _keys[GLFW_KEY_W] ) {
        if(_currVehicle == _raft) {
            _raft->_moveForward(WORLD_SIZE);
            _raft->_paddleForwardLeft();
            _raft->_paddleForwardRight();
        }

        if(_currVehicle == _plane){
            _plane->flyForward();
        }

        // update lookatPoint and recompute orientation
        if(cameraType == 1){
            _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y , _currVehicle->_location.z));
            _camera->recomputeOrientation();
        } else if(cameraType == 3){
            _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y + 5 , _currVehicle->_location.z));
            _camera->recomputeOrientation();
        }
    }
    // go backward
    if( _keys[GLFW_KEY_S] ) {
        if(_currVehicle == _raft){
            _raft->_moveBackward(WORLD_SIZE);
            _raft->_paddleBackward();
        }

        if(_currVehicle == _plane){
            _plane->flyForward();
        }

        if(cameraType == 1){
            _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y , _currVehicle->_location.z));
            _camera->recomputeOrientation();
        } else if(cameraType == 3){
            _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y + 5, _currVehicle->_location.z));
            _camera->recomputeOrientation();
        }
        // update lookout point and recomputer orientation

    }

    if( _keys[GLFW_KEY_1]){
        _currVehicle = _raft;


        if(cameraType == 3){
            _camera->setTheta(_currVehicle->_direction);
            _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y + 5 , _currVehicle->_location.z));
        }
        _camera->recomputeOrientation();
    }

    if( _keys[GLFW_KEY_2]){
        _currVehicle = _plane;
        _currVehicle->_location = _plane->_location;
        _currVehicle->_direction = _plane->_direction;

        if(cameraType == 3){
            _camera->setTheta(_currVehicle->_direction - M_PI/2);
            _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y + 5 , _currVehicle->_location.z));
        }
        
        _camera->recomputeOrientation();
    }
    if( _keys[GLFW_KEY_3]){
        // _currVehicle = last vehicle;
        _camera->recomputeOrientation();

    }

    if(_keys[GLFW_KEY_4]){
        cameraType = 1;
        _changeToArcBallCam();
        _camera->recomputeOrientation();
    }

    if(_keys[GLFW_KEY_5]){
        cameraType = 2;
        _changeToFreeCam();
        _camera->recomputeOrientation();
    }

    if(_keys[GLFW_KEY_6]){
        cameraType = 3;
        _changeToFFPCam();

        if(_currVehicle == _raft) {
            _camera->setTheta(_currVehicle->_direction);
            _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y + 5 , _currVehicle->_location.z));
        }

        if(_currVehicle == _plane) {
            _camera->setTheta(_currVehicle->_direction - M_PI/2);
            _camera->setLookAtPoint(glm::vec3(_currVehicle->_location.x,_currVehicle->_location.y + 5 , _currVehicle->_location.z));
        }

        _camera->recomputeOrientation();
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
        glm::mat4 viewMatrix = _camera->getViewMatrix();


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

    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));

    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.normMtx, normalMtx);


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
