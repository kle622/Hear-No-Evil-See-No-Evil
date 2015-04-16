#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <time.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, txrans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

#include "Library/TimeManager.h"
#include "Library/InitObjects.h"
#include "Library/tiny_obj_loader.h"
#include "Library/GLSL.h"

#include "GameObject/GameObject.h"
#include "GameObject/Player.h"
#include "GameObject/Bunny.h"
#include "GameObject/Shape.h"

#include "Camera/Camera.h"

#define CAMERA_FOV 90
#define CAMERA_NEAR 0.1f
#define CAMERA_FAR 200.0f

#define BUNNY_COUNT 25
#define BUNNY_SPEED 10.0f

#define CAMERA_SPEED 10.0f

GLFWwindow* window;
using namespace std;
using namespace glm;

vector<tinyobj::shape_t> bunny;
vector<tinyobj::shape_t> player;
vector<tinyobj::material_t> materials;

int g_width;
int g_height;
int g_SM = 2;

Camera* camera;
bool cameraFly = false;

glm::vec3 g_light(0, 100, 0);

GLuint ShadeProg;

GLuint posBufObjB = 0;
GLuint norBufObjB = 0;
GLuint indBufObjB = 0;

GLuint posBufObjG = 0;
GLuint norBufObjG = 0;

GLuint posBufObjP = 0;
GLuint norBufObjP = 0;
GLuint indBufObjP = 0;

//Handles to the shader data
GLint h_aPosition;
GLint h_aNormal;
GLint h_uModelMatrix;
GLint h_uViewMatrix;
GLint h_uProjMatrix;
GLint h_uLightPos;
GLint h_cam_pos;
GLint h_uMatAmb, h_uMatDif, h_uMatSpec, h_uMatShine;
GLint h_uShadeM;

float getRand(double M, double N)
{
    float random = (float)(M + (rand() / (RAND_MAX / (N - M))));
    return roundf(random * 1000) / 1000;
}

bool floatCompare(double a, double b)
{
    return fabs(a - b) < .01;
}

int printOglError(const char *file, int line) {
    /* Returns 1 if an OpenGL error occurred, 0 otherwise. */
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        //printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

void SetMaterial(int i) {
    switch (i) {
    case 0: // red (chairs)
        glUniform3f(h_uMatAmb, 0.05f, 0.025f, 0.025f);
        glUniform3f(h_uMatDif, 0.9f, 0.1f, 0.05f);
        glUniform3f(h_uMatSpec, 0.8f, 0.2f, 0.2f);
        glUniform1f(h_uMatShine, 100.0f);
        break;
    case 1: // grey (people + arms)
        glUniform3f(h_uMatAmb, 0.13f, 0.13f, 0.14f);
        glUniform3f(h_uMatDif, 0.3f, 0.3f, 0.4f);
        glUniform3f(h_uMatSpec, 0.3f, 0.3f, 0.4f);
        glUniform1f(h_uMatShine, 150.0f);
        break;
    case 2: // white (bunnies)
        glUniform3f(h_uMatAmb, 0.09f, 0.2f, 0.08f);
        glUniform3f(h_uMatDif, 0.9f, 0.9f, 0.9f);
        glUniform3f(h_uMatSpec, 1.0f, 0.95f, 0.85f);
        glUniform1f(h_uMatShine, 400.0f);
        break;
    case 3: // green (ground)
        glUniform3f(h_uMatAmb, 0.06f, 0.09f, 0.06f);
        glUniform3f(h_uMatDif, 0.2f, 0.95f, 0.1f);
        glUniform3f(h_uMatSpec, 0.8f, 1.0f, 0.8f);
        glUniform1f(h_uMatShine, 4.0f);
        break;
    case 4: // black (hats)
        glUniform3f(h_uMatAmb, 0.08f, 0.08f, 0.08f);
        glUniform3f(h_uMatDif, 0.08f, 0.08f, 0.08f);
        glUniform3f(h_uMatSpec, 0.08f, 0.08f, 0.08f);
        glUniform1f(h_uMatShine, 10.0f);
        break;
    }
}

void initGL()
{
    // Set the background color
    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
    // Enable Z-buffer test
    glEnable(GL_DEPTH_TEST);
    glPointSize(18);

    initModelObject(bunny, &posBufObjB, &norBufObjB, &indBufObjB);
    initModelObject(player, &posBufObjP, &norBufObjP, &indBufObjP);
    initVertexObject(&posBufObjG, &norBufObjG);
}

bool installShaders(const string &vShaderName, const string &fShaderName)
{
    GLint rc;

    // Create shader handles
    GLuint VS = glCreateShader(GL_VERTEX_SHADER);
    GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);

    // Read shader sources
    const char *vshader = GLSL::textFileRead(vShaderName.c_str());
    const char *fshader = GLSL::textFileRead(fShaderName.c_str());
    glShaderSource(VS, 1, &vshader, NULL);
    glShaderSource(FS, 1, &fshader, NULL);

    // Compile vertex shader
    glCompileShader(VS);
    std::cout << "just compiled the v shader" << std::endl;
    printOglError(__FILE__, __LINE__);
    GLSL::printError();
    glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
    GLSL::printShaderInfoLog(VS);
    if (!rc) {
        printf("Error compiling vertex shader %s\n", vShaderName.c_str());
        return false;
    }

    // Compile fragment shader
    glCompileShader(FS);
    std::cout << "just compiled the f shader" << std::endl;
    GLSL::printError();
    glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
    GLSL::printShaderInfoLog(FS);
    if (!rc) {
        printf("Error compiling fragment shader %s\n", fShaderName.c_str());
        return false;
    }

    // Create the program and link
    ShadeProg = glCreateProgram();
    glAttachShader(ShadeProg, VS);
    glAttachShader(ShadeProg, FS);
    glLinkProgram(ShadeProg);
    std::cout << "just linked the shaders" << std::endl;

    GLSL::printError();
    glGetProgramiv(ShadeProg, GL_LINK_STATUS, &rc);
    GLSL::printProgramInfoLog(ShadeProg);
    if (!rc) {
        printf("Error linking shaders %s and %s\n", vShaderName.c_str(), fShaderName.c_str());
        return false;
    }

    /* get handles to attribute data */
    h_aPosition = GLSL::getAttribLocation(ShadeProg, "aPosition");
    h_aNormal = GLSL::getAttribLocation(ShadeProg, "aNormal");
    h_uProjMatrix = GLSL::getUniformLocation(ShadeProg, "uProjMatrix");
    h_uViewMatrix = GLSL::getUniformLocation(ShadeProg, "uViewMatrix");
    h_uModelMatrix = GLSL::getUniformLocation(ShadeProg, "uModelMatrix");
    h_uLightPos = GLSL::getUniformLocation(ShadeProg, "uLightPos");
    h_cam_pos = GLSL::getUniformLocation(ShadeProg, "cam_pos");
    h_uMatAmb = GLSL::getUniformLocation(ShadeProg, "UaColor");
    h_uMatDif = GLSL::getUniformLocation(ShadeProg, "UdColor");
    h_uMatSpec = GLSL::getUniformLocation(ShadeProg, "UsColor");
    h_uMatShine = GLSL::getUniformLocation(ShadeProg, "Ushine");
    h_uShadeM = GLSL::getUniformLocation(ShadeProg, "uShadeModel");

    assert(glGetError() == GL_NO_ERROR);
    return true;
}

void drawGameObjects(vector<shared_ptr <GameObject> >* gameObjects, float time) {
    for (int i = 0; i < (*gameObjects).size(); i++) {
        SetMaterial((*gameObjects)[i].get()->material);
        (*gameObjects)[i].get()->draw();

        if ((*gameObjects)[i].get()->alive) {
            (*gameObjects)[i].get()->move(time);
            if (!dynamic_cast<Shape*>((*gameObjects)[i].get())) { // skip collision for ground
                for (int j = 1; j < (*gameObjects).size() && (*gameObjects)[j]->alive; j++) {
                    if (i != j && (*gameObjects)[j]->alive) {
                        (*gameObjects)[i].get()->collide((*gameObjects)[j].get());
                    }
                }
            }
        }
    }
}

void beginDrawGL() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our GLSL program
    glUseProgram(ShadeProg);
    glUniform3f(h_uLightPos, g_light.x, g_light.y, g_light.z);
    glUniform1i(h_uShadeM, g_SM);
    glUniform3f(h_cam_pos, camera->position.x,
        camera->position.y, camera->position.z);
    GLSL::enableVertexAttribArray(h_aPosition);
    GLSL::enableVertexAttribArray(h_aNormal);
}

void endDrawGL() {
    GLSL::disableVertexAttribArray(h_aPosition);
    GLSL::disableVertexAttribArray(h_aNormal);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(0);
    assert(glGetError() == GL_NO_ERROR);
}

void window_size_callback(GLFWwindow* window, int w, int h){
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    g_width = w;
    g_height = h;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        cameraFly = !cameraFly;
    }
}

void getWindowinput(GLFWwindow* window, double deltaTime) {
    float forwardYVelocity = 0;
    float sideYVelocity = 0;

    if (cameraFly) {
        forwardYVelocity = camera->direction.y * CAMERA_SPEED * deltaTime;
        sideYVelocity = camera->rightV.y * CAMERA_SPEED * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 velocity = glm::vec3(camera->rightV.x * CAMERA_SPEED * deltaTime, 
            sideYVelocity, camera->rightV.z * CAMERA_SPEED * deltaTime);
        camera->position -= velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 velocity = glm::vec3(camera->rightV.x * CAMERA_SPEED * deltaTime, 
            sideYVelocity, camera->rightV.z * CAMERA_SPEED * deltaTime);
        camera->position += velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        vec3 velocity = glm::vec3(camera->direction.x * CAMERA_SPEED * deltaTime,
            forwardYVelocity, camera->direction.z * CAMERA_SPEED * deltaTime);
        camera->position += velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        vec3 velocity = glm::vec3(camera->direction.x * CAMERA_SPEED * deltaTime,
            forwardYVelocity, camera->direction.z * CAMERA_SPEED * deltaTime);
        camera->position -= velocity;
    }
}

int main(int argc, char **argv)
{
    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Open a window and create its OpenGL context
    g_width = 1600;
    g_height = 900;
    window = glfwCreateWindow(g_width, g_height, "bunny and ground", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }

    printf("correct window address %p\n", window);
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    // Initialize GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetCursorPos(window, g_width / 2, g_height / 2);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    loadShapes("bunny.obj", bunny, materials);
    loadShapes("godzilla.obj", player, materials);
    initGL();
    installShaders("vert.glsl", "frag.glsl");

    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
    srand(time(NULL));

    //initialize game objects
    vector<shared_ptr <GameObject> > gameObjects;
    int currBunnies = 0;

    gameObjects.push_back(shared_ptr<GameObject>(new Shape(
        h_uModelMatrix, //model handle
        vec3(0), //position
        0, //rotation
        1, //scale
        vec3(1, 0, 0), //direction
        0, //velocity
        6, //indices
        posBufObjG, //position buffer
        norBufObjG, //normal buffer
        h_aPosition, //position handle
        h_aNormal, //normal handle
        3 //material
    )));

    Player* playerObject = new Player(
        h_uModelMatrix,
        vec3(0, 0, 0),
        0,
        1,
        vec3(1, 0, 0),
        CAMERA_SPEED,
        vec3(2.5, 2.5, 2.5),
        (int)player[0].mesh.indices.size(),
        posBufObjP,
        norBufObjP,
        indBufObjP,
        h_aPosition,
        h_aNormal,
        0
    );

    gameObjects.push_back(shared_ptr<GameObject>(playerObject));

    //initialize the camera
    camera = new Camera(window, h_uViewMatrix, h_uProjMatrix,
        vec3(0, 1, 0), CAMERA_FOV,
        (float)g_width / g_height, CAMERA_NEAR, CAMERA_FAR);

    double timeCounter = 0;
    do{
        //timer stuff
        TimeManager::Instance().CalculateFrameRate(true);
        printf("score: %d\nbunnies: %d\n", playerObject->score, currBunnies - playerObject->score);
        double deltaTime = TimeManager::Instance().DeltaTime;
        double currentTime = TimeManager::Instance().CurrentTime;
        timeCounter += deltaTime;

        //get mouse/keyboard input
        getWindowinput(window, deltaTime);

        //add bunny every 3 seconds
        if (timeCounter > 3.0f && currBunnies < BUNNY_COUNT) {
            timeCounter -= 3.0f;
            currBunnies++;

            gameObjects.push_back(shared_ptr<GameObject>(new Bunny(
                    h_uModelMatrix, //model handle
                    vec3(getRand(-29, 29), 0, getRand(-29, 29)), //position
                    0, //rotation
                    1, //scale
                    vec3(getRand(-1, 1), 0, getRand(-1, 1)), //direction
                    BUNNY_SPEED, //speed
                    vec3(1.5, 1.5, 1.5), //bounding box
                    (int)bunny[0].mesh.indices.size(), //num indices on mesh
                    posBufObjB, //position buffer
                    norBufObjB, //normal buffer
                    indBufObjB, //index buffer
                    h_aPosition, //position handle
                    h_aNormal, //normal handle
                    2 //material
            )));
        }

        beginDrawGL();
        camera->setProjection(g_width, g_height);
        camera->setView(g_width, g_height);
        playerObject->position.x = camera->position.x;
        playerObject->position.y = camera->position.y - 1;
        playerObject->position.z = camera->position.z;
        playerObject->rotation = atan2f(camera->direction.x, camera->direction.z) * 180 / M_PI + 180;
        drawGameObjects(&gameObjects, deltaTime);
        endDrawGL();

        glfwSwapBuffers(window);
        glfwPollEvents();
    } 
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS 
        && glfwWindowShouldClose(window) == 0);

    glfwTerminate();

    return 0;
}