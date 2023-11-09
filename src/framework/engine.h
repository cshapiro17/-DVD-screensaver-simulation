#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

#include <vector>
#include <memory>
#include <iostream>
#include <GLFW/glfw3.h>

#include "shaderManager.h"
#include "../shapes/circle.h"
#include "../shapes/rect.h"
#include "../shapes/shape.h"

using std::vector, std::unique_ptr, std::make_unique, glm::ortho, glm::mat4, glm::vec3, glm::vec4;

/**
 * @brief The Engine class.
 * @details The Engine class is responsible for initializing the GLFW window, loading shaders, and rendering the game state.
 */
class Engine {
    private:
        /// @brief The actual GLFW window.
        GLFWwindow* window{};

        /// @brief The width and height of the window.
        const unsigned int WIDTH = 800, HEIGHT = 600; // Window dimensions

        /// @brief Responsible for loading and storing all the shaders used in the project.
        /// @details Initialized in initShaders()
        unique_ptr<ShaderManager> shaderManager;

        // Shapes
        vector<unique_ptr<Circle>> bubbles;
        const int RADIUS = 50;

        // Shaders
        Shader shapeShader;

        double mouseX, mouseY;

    public:
        /// @brief Constructor for the Engine class.
        /// @details Initializes window and shaders.
        Engine();

        /// @brief Destructor for the Engine class.
        ~Engine();

        /// @brief Initializes the GLFW window.
        /// @return 0 if successful, -1 otherwise.
        unsigned int initWindow(bool debug = false);

        /// @brief Loads shaders from files and stores them in the shaderManager.
        /// @details Renderers are initialized here.
        void initShaders();

        /// @brief Initializes the shapes to be rendered.
        void initShapes();

        /// @brief Processes input from the user.
        /// @details (e.g. keyboard input, mouse input, etc.)
        void processInput();

        /// @brief Updates the game state.
        /// @details (e.g. collision detection, delta time, etc.)
        void update();

        /// @brief Renders the game state.
        /// @details Displays/renders objects on the screen.
        void render();

        /* deltaTime variables */
        float deltaTime = 0.0f; // Time between current frame and last frame
        float lastFrame = 0.0f; // Time of last frame (used to calculate deltaTime)

        // -----------------------------------
        // Getters
        // -----------------------------------

        /// @brief Returns true if the window should close.
        /// @details (Wrapper for glfwWindowShouldClose()).
        /// @return true if the window should close
        /// @return false if the window should not close
        bool shouldClose();

        /// Projection matrix used for 2D rendering (orthographic projection).
        /// We don't have to change this matrix since the screen size never changes.
        /// OpenGL uses the projection matrix to map the 3D scene to a 2D viewport.
        /// The projection matrix transforms coordinates in the camera space into normalized device coordinates (view space to clip space).
        /// @note The projection matrix is used in the vertex shader.
        // 1st quadrant
        mat4 PROJECTION = ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT), -1.0f, 1.0f);
        // 4th quadrant
        // mat4 PROJECTION = ortho(0.0f, static_cast<float>(WIDTH), static_cast<float>(HEIGHT), 0.0f, -1.0f, 1.0f);

        /// @brief Checks for collisions between all bubbles
        void checkCollisions();

        /// @brief Prevents bubbles from going off screen
        void checkBounds(unique_ptr<Circle> &bubble) const;

};

#endif //GRAPHICS_ENGINE_H