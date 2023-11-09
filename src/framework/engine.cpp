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
    shapeShader = this->shaderManager->loadShader("../res/shaders/circle.vert",
                                                  "../res/shaders/circle.frag",
                                                  nullptr, "circle");
    shapeShader.use();
    shapeShader.setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {
    int numberOfBubbles = 30;
    float minRadius = 30;
    float maxRadius = 40;
    float maxSpeed = 100;

    for (int i = 0; i < numberOfBubbles; ++i) {
        float x = rand() % WIDTH;
        float y = rand() % HEIGHT;
        float radius = rand() % int(maxRadius - minRadius) + minRadius;
        vec2 position(x, y);
        vec2 velocity(rand() % int(maxSpeed), rand() % int(maxSpeed));
        // get 3 random floats between 0 and 1 for RGB
        vec4 randomColor(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 120 + 135);
        unique_ptr<Circle> bubble = make_unique<Circle>(shapeShader, position, radius, velocity, randomColor);
        bubble->setVelocity(velocity);
        bubbles.push_back(std::move(bubble));
    }
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


void Engine::checkBounds(unique_ptr<Circle> &bubble) const {
    vec2 position = bubble->getPos();
    vec2 velocity = bubble->getVelocity();
    float bubbleRadius = bubble->getRadius();

    position += velocity * deltaTime;

    // If any bubble hits the edges of the screen, bounce it in the other direction
    if (position.x - bubbleRadius <= 0) {
        position.x = bubbleRadius;
        velocity.x = -velocity.x;
    }
    if (position.x + bubbleRadius >= WIDTH) {
        position.x = WIDTH - bubbleRadius;
        velocity.x = -velocity.x;
    }
    if (position.y - bubbleRadius <= 0) {
        position.y = bubbleRadius;
        velocity.y = -velocity.y;
    }
    if (position.y + bubbleRadius >= HEIGHT) {
        position.y = HEIGHT - bubbleRadius;
        velocity.y = -velocity.y;
    }

    bubble->setPos(position);
    bubble->setVelocity(velocity);
}

void Engine::update() {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

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
}

void Engine::render() {
    glClearColor(BLACK.red, BLACK.green, BLACK.blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shapeShader.use();

    for (unique_ptr<Circle>& bubble : bubbles) {
        bubble->setUniforms();
        bubble->draw();
    }

    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}