#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <time.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, txrans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
#include <memory>

#include "Library/TimeManager.h"
#include "Library/InitObjects.h"
#include "Library/tiny_obj_loader.h"
#include "Library/GLSL.h"
#include "Library/GLError.h"

#include "GameObject/GameObject.h"
#include "GameObject/Player.h"
#include "GameObject/Shape.h"
#include "GameObject/Wall.h"
#include "GameObject/Handles.h"
#include "GameObject/Mesh.h"
#include "Camera/Camera.h"
#include "Path/Path.h"

#include "WorldGrid/WorldGrid.h"

#define WORLD_WIDTH 60
#define WORLD_HEIGHT 60

#define CAMERA_FOV 45
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
vector<tinyobj::shape_t> wall;

int g_width;
int g_height;
int g_SM = 2;

Camera* camera;
Handles handles;
Mesh bunnyMesh;
Mesh playerMesh;
Mesh cubeMesh;
Shape *ground;
bool cameraFly = false;

glm::vec3 g_light(0, 100, 0);

GLuint posBufObjG = 0;
GLuint norBufObjG = 0;

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
        glUniform3f(handles.uMatAmb, 0.05f, 0.025f, 0.025f);
        glUniform3f(handles.uMatDif, 0.9f, 0.1f, 0.05f);
        glUniform3f(handles.uMatSpec, 0.8f, 0.2f, 0.2f);
        glUniform1f(handles.uMatShine, 100.0f);
        break;
    case 1: // grey (people + arms)
        glUniform3f(handles.uMatAmb, 0.13f, 0.13f, 0.14f);
        glUniform3f(handles.uMatDif, 0.3f, 0.3f, 0.4f);
        glUniform3f(handles.uMatSpec, 0.3f, 0.3f, 0.4f);
        glUniform1f(handles.uMatShine, 150.0f);
        break;
    case 2: // white (bunnies)
        glUniform3f(handles.uMatAmb, 0.09f, 0.2f, 0.08f);
        glUniform3f(handles.uMatDif, 0.9f, 0.9f, 0.9f);
        glUniform3f(handles.uMatSpec, 1.0f, 0.95f, 0.85f);
        glUniform1f(handles.uMatShine, 400.0f);
        break;
    case 3: // green (ground)
        glUniform3f(handles.uMatAmb, 0.06f, 0.09f, 0.06f);
        glUniform3f(handles.uMatDif, 0.2f, 0.95f, 0.1f);
        glUniform3f(handles.uMatSpec, 0.8f, 1.0f, 0.8f);
        glUniform1f(handles.uMatShine, 4.0f);
        break;
    case 4: // black (hats)
        glUniform3f(handles.uMatAmb, 0.08f, 0.08f, 0.08f);
        glUniform3f(handles.uMatDif, 0.08f, 0.08f, 0.08f);
        glUniform3f(handles.uMatSpec, 0.08f, 0.08f, 0.08f);
        glUniform1f(handles.uMatShine, 10.0f);
        break;
    }
}

void initGL() {
    // Set the background color
    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
    // Enable Z-buffer test
    glEnable(GL_DEPTH_TEST);
    glPointSize(18);
    initVertexObject(&posBufObjG, &norBufObjG);
}


void drawGameObjects(WorldGrid* gameObjects, float time) {
    ground->draw();

    for (int i = 0; i < gameObjects->grid.size(); i ++) {
        for (int j = 0; j < gameObjects->grid[i].size(); j++) {
            for (int k = 0; k < gameObjects->grid[i][j].size(); k++) {
                if (gameObjects->grid[i][j][k]->alive) {
                    gameObjects->grid[i][j][k]->move(time);
                    SetMaterial(gameObjects->grid[i][j][k]->material);
                    gameObjects->grid[i][j][k]->draw();

                    vector<shared_ptr<GameObject>> proximity = 
                        gameObjects->getCloseObjects(gameObjects->grid[i][j][k]);

                    for (int r = 0; r < proximity.size(); r++) {
                        if (gameObjects->grid[i][j][k].get() != proximity[r].get()) {
                            gameObjects->grid[i][j][k]->collide(proximity[r].get());
                        }
                    }
                }
            }
        }
    }

    gameObjects->update();
}

void beginDrawGL() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our GLSL program
    glUseProgram(handles.prog);
    glUniform3f(handles.uLightPos, g_light.x, g_light.y, g_light.z);
    glUniform3f(handles.uCamPos, camera->position.x,
        camera->position.y, camera->position.z);
    GLSL::enableVertexAttribArray(handles.aPosition);
    GLSL::enableVertexAttribArray(handles.aNormal);
}

void endDrawGL() {
    GLSL::disableVertexAttribArray(handles.aPosition);
    GLSL::disableVertexAttribArray(handles.aNormal);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(0);
    checkGLError();
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
    g_width = 1080;
    g_height = 720;
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
    initGL();
#ifdef _WIN32
    handles.installShaders(getResourcePath("shaders\\vert.glsl"), getResourcePath("shaders\\frag.glsl"));
#else
    handles.installShaders(getResourcePath("shaders/vert.glsl"), getResourcePath("shaders/frag.glsl"));
#endif

#ifdef _WIN32
    bunnyMesh.loadShapes(getResourcePath("models\\bunny.obj"));
    playerMesh.loadShapes(getResourcePath("models\\godzilla.obj"));
    cubeMesh.loadShapes(getResourcePath("models\\cube.obj"));
#else
    bunnyMesh.loadShapes(getResourcePath("models/bunny.obj"));
    playerMesh.loadShapes(getResourcePath("models/godzilla.obj"));
    cubeMesh.loadShapes(getResourcePath("models/cube.obj"));
#endif
    
    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
    srand(time(NULL));

    //initialize game objects
    //vector<shared_ptr <GameObject> > gameObjects;
    WorldGrid gameObjects(WORLD_WIDTH, WORLD_HEIGHT);

    int currBunnies = 0;

    ground = new Shape(
        &handles, //model handle
        vec3(0), //position
        0, //rotation
        vec3(1.0,1.0,1.0), //scale
        vec3(1, 0, 0), //direction
        0, //velocity
	6, //indices
	posBufObjG, 
	norBufObjG,
        3 //material
	);

    Player* playerObject = new Player(
      &playerMesh,
      &handles,
      vec3(0, 0, 0),
      0,
      vec3(1.0, 1.0, 1.0), //scale
      vec3(1, 0, 0),
      CAMERA_SPEED,
      vec3(2.5, 2.5, 2.5),
      1,
      1
   );

   gameObjects.add(shared_ptr<GameObject>(playerObject));

    // Read in from text file
    int levelDesign[50][50]; 
    char ch;
    fstream fin(getResourcePath("LevelDesign.txt"), fstream::in);
    int i = 0, j = 0;
    while (fin >> noskipws >> ch) {
      if (ch == '\n') {
        j = 0;
        i++;
      }
      else {
        levelDesign[i][j] = ch - '0';
        j++;
      }
    }
    for (i = 0; i < 50; i++) {
      cout << '\n';
      for (j = 0; j < 50; j++) {
        cout << levelDesign[i][j];
      }
    }
    cout << '\n';
    // Create the wall objects
    for (i = 0; i < 50; i++) {
      for (j = 0; j < 50; j++) {
        if (levelDesign[i][j]) {
          gameObjects.add(shared_ptr<GameObject>(new Wall(
	    &cubeMesh,
	    &handles, //model handle
            vec3((i-25), 0, (j-25)), //position
            0, //rotation
            vec3(1.0, 5.0, 1.0), //scale
            vec3(1, 0, 0), //direction
            0, //speed
            vec3(1, 1, 1), //bounding box
            1, //scanRadius
            1  //material
            )));
        }
      }
    }
    
    //initialize the camera
    camera = new Camera(window, handles.uViewMatrix, handles.uProjMatrix,
        vec3(0, 1, 0), CAMERA_FOV,
        (float)g_width / g_height, CAMERA_NEAR, CAMERA_FAR);

    double timeCounter = 0;
    do{
        //timer stuff
        TimeManager::Instance().CalculateFrameRate(true);
        double deltaTime = TimeManager::Instance().DeltaTime;
        double currentTime = TimeManager::Instance().CurrentTime;
        timeCounter += deltaTime;

        //get mouse/keyboard input
        getWindowinput(window, deltaTime);

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
