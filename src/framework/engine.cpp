#include "engine.h"
#include <string>

enum state {play, pause, over};
state screen;

const color WHITE(1, 1, 1);
const color BLACK(0, 0, 0);
const color BLUE(0, 0, 1);
const color YELLOW(1, 1, 0);
const color RED(1, 0, 0);

Engine::Engine() : keys(){
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
    // Load shader manager
    shaderManager = make_unique<ShaderManager>();

    // Load shader into shader manager and retrieve it
    shapeShader = this->shaderManager->loadShader("../res/shaders/shape.vert",
                                                  "../res/shaders/shape.frag",
                                                  nullptr, "shape");

    // Configure text shader and renderer
    textShader = shaderManager->loadShader("../res/shaders/text.vert", "../res/shaders/text.frag", nullptr, "text");
    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", 24);

    textShader.use().setVector2f("vertex", vec4(100, 100, .5, .5));
    shapeShader.use();
    shapeShader.setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {

    // Make a 50x30 white rectangle
    dvd = make_unique<Rect>(shapeShader, vec2(WIDTH / 2, HEIGHT / 2), vec2(50, 30), vec2(100, 100), WHITE);
}

void Engine::processInput() {
    glfwPollEvents();

    // Set keys to true if pressed, false if released
    for (int key = 0; key < 1024; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            keys[key] = true;
        else if (glfwGetKey(window, key) == GLFW_RELEASE)
            keys[key] = false;
    }

    if (screen == play && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        screen = pause;
    }

    if (screen == pause && glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
        screen = play;
    }

    // Close window if escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Mouse position saved to check for collisions
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouseY = HEIGHT - mouseY; // make sure mouse y-axis isn't flipped

    // Allow the user to change the velocity of dvd logo with the arrow keys
    if (screen == play && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        vec2 velocity = dvd->getVelocity();
        velocity.y = velocity.y + 1;
        dvd->setVelocity(velocity);
    }
    if (screen == play && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        vec2 velocity = dvd->getVelocity();
        velocity.y = velocity.y - 1;
        dvd->setVelocity(velocity);
    }
    if (screen == play && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        vec2 velocity = dvd->getVelocity();
        velocity.x = velocity.x - 1;
        dvd->setVelocity(velocity);
    }
    if (screen == play && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        vec2 velocity = dvd->getVelocity();
        velocity.x = velocity.x + 1;
        dvd->setVelocity(velocity);
    }

    // Change the color of the rectangle each time the user clicks the mouse
    bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    if (screen == play && mousePressed) {
        color color = {float(rand() % 10 / 10.0), float(rand() % 10 / 10.0), float(rand() % 10 / 10.0), 1.0f};
        dvd->setColor(color);
    }
}


void Engine::checkBounds(unique_ptr<Rect> &dvd) {
    vec2 position = dvd->getPos();
    vec2 velocity = dvd->getVelocity();
    vec2 size = dvd->getSize();

    position += velocity * deltaTime;

    // If any bubble hits the edges of the screen, bounce it in the other direction
    if (position.x - (size.x / 2) <= 0 && screen == play) {
        position.x = (size.x / 2);
        velocity.x = -velocity.x;
        wallsHit++;
    }
    if (position.x + (size.x / 2) >= WIDTH && screen == play) {
        position.x = WIDTH - (size.x / 2);
        velocity.x = -velocity.x;
        wallsHit++;
    }
    if (position.y - (size.y / 2) <= 0 && screen == play) {
        position.y = (size.y / 2);
        velocity.y = -velocity.y;
        wallsHit++;
    }
    if (position.y + (size.y / 2) >= HEIGHT && screen == play) {
        position.y = HEIGHT - (size.y / 2);
        velocity.y = -velocity.y;
        wallsHit++;
    }

    // Determine if a corner has been hit
    if (position.x - (size.x / 2) <= 0 && position.y - (size.y / 2) <= 0 && screen == play) {
        cornersHit++;
    }
    if (position.x + (size.x / 2) >= WIDTH && position.y - (size.y / 2) <= 0 && screen == play) {
        cornersHit++;
    }
    if (position.x - (size.x / 2) <= 0 && position.y + (size.y / 2) >= HEIGHT && screen == play) {
        cornersHit++;
    }
    if (position.x + (size.x / 2) >= WIDTH && position.y + (size.y / 2) >= HEIGHT && screen == play) {
        cornersHit++;
    }

    dvd->setPos(position);
    dvd->setVelocity(velocity);
}

void Engine::update() {

    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Prevent dvd from moving offscreen
    if (screen == play) {
        checkBounds(dvd);
    }
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

    // Render differently depending on screen
    switch (screen) {
        case pause: {
            string message1 = "Press backspace to return";
            string message2 = "Walls Hit: " + std::to_string(wallsHit);
            string message3 = "Corners Hit: " + std::to_string(cornersHit);

            // Display the message on the screen
            fontRenderer->renderText(message1, (WIDTH / 2) - 100, (HEIGHT / 2) + 50, 0.5, vec3{1, 1, 1});
            fontRenderer->renderText(message2, (WIDTH / 2) - 100, (HEIGHT / 2) + 20, 0.5, vec3{1, 1, 1});
            fontRenderer->renderText(message3, (WIDTH / 2) - 100, (HEIGHT / 2), 0.5, vec3{1, 1, 1});
            break;
        }
        case play: {
            string message = "Press P to pause";

            // Display rectangle
            dvd->setUniforms();
            dvd->draw();

            // Display the message on the screen
            fontRenderer->renderText(message, (WIDTH / 2) - 100, (HEIGHT / 2), 0.5, vec3{1, 1, 1});
            break;
        }
        case over: {

            /*
            string message = "You win!";
            // Display the message on the screen
            fontRenderer->renderText(message, (width / 2) - 25, (height / 2), 0.5, vec3{1, 1, 1});
            break;
             */
        }
    }
    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}