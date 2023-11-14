#include "engine.h"

const color WHITE(1, 1, 1);
const color BLACK(0, 0, 0);
const color BLUE(0, 0, 1);
const color YELLOW(1, 1, 0);
const color RED(1, 0, 0);

Engine::Engine() {
    this->initWindow();
    this->initShaders();
    this->initShapes();
}

Engine::~Engine() {}

unsigned int Engine::initWindow(bool debug) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(WIDTH, HEIGHT, "engine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // OpenGL configuration
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    return 0;
}

void Engine::initShaders() {
    shaderManager = make_unique<ShaderManager>();
    shapeShader = this->shaderManager->loadShader("../res/shaders/shape.vert",
                                                  "../res/shaders/shape.frag",
                                                  nullptr, "shape");
    shapeShader.use();
    shapeShader.setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {

    // Make a 50x30 white rectangle
    dvd = make_unique<Rect>(shapeShader, vec2(WIDTH / 2, HEIGHT / 2), vec2(50, 30), vec2(100, 100), WHITE);
}

void Engine::processInput() {
    glfwPollEvents();

    // Close window if escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Mouse position saved to check for collisions
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouseY = HEIGHT - mouseY; // make sure mouse y-axis isn't flipped
}


void Engine::checkBounds(unique_ptr<Rect> &dvd) const {
    vec2 position = dvd->getPos();
    vec2 velocity = dvd->getVelocity();
    vec2 size = dvd->getSize();

    position += velocity * deltaTime;

    // If any bubble hits the edges of the screen, bounce it in the other direction
    if (position.x - (size.x / 2) <= 0) {
        position.x = (size.x / 2);
        velocity.x = -velocity.x;
    }
    if (position.x + (size.x / 2) >= WIDTH) {
        position.x = WIDTH - (size.x / 2);
        velocity.x = -velocity.x;
    }
    if (position.y - (size.y / 2) <= 0) {
        position.y = (size.y / 2);
        velocity.y = -velocity.y;
    }
    if (position.y + (size.y / 2) >= HEIGHT) {
        position.y = HEIGHT - (size.y / 2);
        velocity.y = -velocity.y;
    }

    dvd->setPos(position);
    dvd->setVelocity(velocity);
}

void Engine::update() {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    /*
    for (unique_ptr<Circle> &bubble: bubbles) {
        // Prevent bubbles from moving off screen
        checkBounds(bubble);
        // Check for collisions
        for (unique_ptr<Circle> &other: bubbles) {
            if (bubble != other && bubble->isOverlapping(*other)) {
                bubble->bounce(*other);
            }
        }
    }
    */

    // Need to adapt for a rectangle
    // Prevent dvd from moving offscreen
    checkBounds(dvd);
}

void Engine::render() {
    glClearColor(BLACK.red, BLACK.green, BLACK.blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shapeShader.use();

    /*
    for (unique_ptr<Circle>& bubble : bubbles) {
        bubble->setUniforms();
        bubble->draw();
    }
    */

    dvd->setUniforms();
    dvd->draw();

    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}