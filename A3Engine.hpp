#ifndef LAB05_LAB05_ENGINE_HPP
#define LAB05_LAB05_ENGINE_HPP

#include <CSCI441/Camera.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>
#include "Plane.hpp"
#include "Car.hpp"
#include "FFPCam.hpp"

#include <vector>
#include "Raft.hpp"

class A3Engine : public CSCI441::OpenGLEngine {
public:
    A3Engine();
    ~A3Engine();

    void run() final;

    /// \desc handle any key events inside the engine
    /// \param key key as represented by GLFW_KEY_ macros
    /// \param action key event action as represented by GLFW_ macros
    void handleKeyEvent(GLint key, GLint action);

    /// \desc handle any mouse button events inside the engine
    /// \param button mouse button as represented by GLFW_MOUSE_BUTTON_ macros
    /// \param action mouse event as represented by GLFW_ macros
    void handleMouseButtonEvent(GLint button, GLint action);

    /// \desc handle any cursor movement events inside the engine
    /// \param currMousePosition the current cursor position
    void handleCursorPositionEvent(glm::vec2 currMousePosition);

    /// \desc value off-screen to represent mouse has not begun interacting with window yet
    static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;

private:
    void _setupGLFW() final;
    void _setupOpenGL() final;
    void _setupShaders() final;
    void _setupBuffers() final;
    void _setupScene() final;

    void _cleanupBuffers() final;
    void _cleanupShaders() final;

    /// \desc draws everything to the scene from a particular point of view
    /// \param viewMtx the current view matrix for our camera
    /// \param projMtx the current projection matrix for our camera
    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const;
    /// \desc handles moving our FreeCam as determined by keyboard input
    void _updateScene();

    void _changeToFreeCam();
    void _changeToArcBallCam();
    void _changeToFFPCam();

    /// \desc tracks the number of different keys that can be present as determined by GLFW
    static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;
    /// \desc boolean array tracking each key state.  if true, then the key is in a pressed or held
    /// down state.  if false, then the key is in a released state and not being interacted with
    GLboolean _keys[NUM_KEYS];

    /// \desc last location of the mouse in window coordinates
    glm::vec2 _mousePosition;
    /// \desc current state of the left mouse button
    GLint _leftMouseButtonState;

    Vehicle* _currVehicle;

    bool FFPToggle;

    Plane* _plane;
    /// \desc the static fixed camera in our world
    CSCI441::Camera* _camera;
    CSCI441::Camera* _cameraFFP;
    /// \desc pair of values to store the speed the camera can move/rotate.
    /// \brief x = forward/backward delta, y = rotational delta
    glm::vec2 _cameraSpeed;

    Raft* _raft;

    Car* _car;

    /// \desc the size of the world (controls the ground size and locations of buildings)
    static constexpr GLfloat WORLD_SIZE = 55.0f;
    /// \desc VAO for our ground
    GLuint _groundVAO;
    /// \desc the number of points that make up our ground object
    GLsizei _numGroundPoints;
    //// \desc VAO for our buildings
    GLuint _buildingVAO;
    //// \desc VAO for our pyramid
    GLuint _pyramidVAO;

    /// \desc creates the ground, building, and pyramid VAO
    void _createGroundBuffers();
    void _createBuildingBuffers();
    void _createPyramidBuffers();

    struct PyramidData {
        glm::mat4 modelMatrix;
        glm::vec3 color;
    };

    struct BuildingData {
        glm::mat4 modelMatrix;
        glm::vec3 color;
    };

    //// \desc Struct to hold our Point Light information
    struct PointLight {
        glm::vec3 position;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };


    std::vector<PyramidData> _pyramids;

    std::vector<BuildingData> _buildings;


    /// \desc generates building information to make up our scene
    void _generateEnvironment();


    /// \desc shader program that performs lighting
    CSCI441::ShaderProgram* _lightingShaderProgram = nullptr;   // the wrapper for our shader program
    /// \desc stores the locations of all of our shader uniforms
    struct LightingShaderUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;
        /// \desc material diffuse color location
        GLint materialColor;
        // TODO #1: add new uniforms
        GLuint normMtx;
        //// \desc holds direction of our lighting
        GLuint lightDirection;
        //// \desc holds color of our lighting
        GLuint lightColor;
        //// \desc holds the current position of the camera
        GLuint cameraPosition;
        //// \desc holds our model matrix handle
        GLuint modelMatrix;
    } _lightingShaderUniformLocations;
    /// \desc stores the locations of all of our shader attributes
    struct LightingShaderAttributeLocations {
        /// \desc vertex position location
        GLint vPos;
        // TODO #2: add new attributes
        GLuint vecNormal;


    } _lightingShaderAttributeLocations;

    /// \desc precomputes the matrix uniforms CPU-side and then sends them
    /// to the GPU to be used in the shader for each vertex.  It is more efficient
    /// to calculate these once and then use the resultant product in the shader.
    /// \param modelMtx model transformation matrix
    /// \param viewMtx camera view matrix
    /// \param projMtx camera projection matrix
    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

};
//// CALLBACKS
void lab05_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods );
void lab05_engine_cursor_callback(GLFWwindow *window, double x, double y );
void lab05_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods );

#endif // LAB05_LAB05_ENGINE_HPP
