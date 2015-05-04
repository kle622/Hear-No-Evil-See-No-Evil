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
#include "GameObject/Guard.h"
#include "GameObject/Player.h"
#include "GameObject/Shape.h"
#include "GameObject/Wall.h"
#include "GameObject/Handles.h"
#include "GameObject/Mesh.h"
#include "Camera/Camera.h"
#include "Camera/Camera3DPerson.h"
#include "Path/Path.h"
#include "WorldGrid/WorldGrid.h"

#define WORLD_WIDTH 3000
#define WORLD_HEIGHT 3000
#define TEST_WORLD 50

#define CAMERA_FOV 60
#define CAMERA_NEAR 0.1f
#define CAMERA_FAR 200.0f
#define CAMERA_ZOOM 3.0f
#define CAMERA_SPEED 10.0f

#define GUARD_SPEED 5.0f

GLFWwindow* window;
using namespace std;
using namespace glm;

vector<tinyobj::shape_t> player;
vector<tinyobj::shape_t> guard;
vector<tinyobj::material_t> materials;
vector<tinyobj::shape_t> wall;

int g_width;
int g_height;

Camera* camera;
Camera3DPerson *camera3DPerson;
Player* playerObject;
vec3 oldPosition;
Handles handles;
Mesh guardMesh;
Mesh playerMesh;
Mesh cubeMesh;
Shape *ground;
bool cameraFly = false;

glm::vec3 g_light(0, 100, 0);

GLuint posBufObjG = 0;
GLuint norBufObjG = 0;

double deltaTime;

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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Enable Z-buffer test
    glEnable(GL_DEPTH_TEST);
    glPointSize(18);
    initVertexObject(&posBufObjG, &norBufObjG);
}

void getWindowinput(GLFWwindow* window, double deltaTime) {
    float forwardYVelocity = 0;
    float sideYVelocity = 0;
    glm::vec3 forward = camera3DPerson->getForward();
    glm::vec3 strafe = camera3DPerson->getStrafe();
    glm::vec3 up = camera3DPerson->getUp();
    oldPosition = playerObject->position;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 velocity = glm::vec3(strafe.x * CAMERA_SPEED * deltaTime, 
            sideYVelocity, strafe.z * CAMERA_SPEED * deltaTime);
        playerObject->position -= velocity;
        glm::vec3 forward = camera3DPerson->getForward();
		playerObject->rotation = atan2f(-velocity.x, -velocity.z) * 180 / M_PI + 180;
        //camera3DPerson->setView();
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 velocity = glm::vec3(strafe.x * CAMERA_SPEED * deltaTime, 
            sideYVelocity, strafe.z * CAMERA_SPEED * deltaTime);
        playerObject->position += velocity;
        glm::vec3 forward = camera3DPerson->getForward();
		playerObject->rotation = atan2f(velocity.x, velocity.z) * 180 / M_PI + 180;
        //camera3DPerson->setView();
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        vec3 velocity = glm::vec3(forward.x * CAMERA_SPEED * deltaTime,
            forwardYVelocity, forward.z * CAMERA_SPEED * deltaTime);
        playerObject->position += velocity;
        glm::vec3 forward = camera3DPerson->getForward();
		playerObject->rotation = atan2f(velocity.x, velocity.z) * 180 / M_PI + 180;
        //camera3DPerson->setView();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        vec3 velocity = glm::vec3(forward.x * CAMERA_SPEED * deltaTime,
            forwardYVelocity, forward.z * CAMERA_SPEED * deltaTime);
        playerObject->position -= velocity;
        glm::vec3 forward = camera3DPerson->getForward();
		playerObject->rotation = atan2f(-velocity.x, -velocity.z) * 180 / M_PI + 180;
        //camera3DPerson->setView();
    }
}

void drawGameObjects(WorldGrid* gameObjects, float time) {
    SetMaterial(ground->material);
    ground->draw();
    Guard *guard;

    for (int i = 0; i < gameObjects->list.size(); i++) {
        SetMaterial(gameObjects->list[i]->material);
        gameObjects->list[i]->draw();
        gameObjects->list[i]->move(time);
        vector<shared_ptr<GameObject>> proximity = 
            gameObjects->getCloseObjects(gameObjects->list[i]);

        if (guard = dynamic_cast<Guard*>(gameObjects->list[i].get())) {
            guard->detect(playerObject);
        }

        for (int j = 0; j < proximity.size(); j++) {
            if (gameObjects->list[i] != proximity[j]) {
                if (gameObjects->list[i]->collide(proximity[j].get())) {
                    if (dynamic_cast<Player*>(gameObjects->list[i].get())) {
                        gameObjects->list[i]->position = oldPosition;
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
    glUniform3f(handles.uCamPos, camera3DPerson->eye.x,
        camera3DPerson->eye.y, camera3DPerson->eye.z);
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
    if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera3DPerson->zoom *= 0.9;
      //camera3DPerson->setView();
    }
    if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera3DPerson->zoom *= 1.1;
      //camera3DPerson->setView();
    }
}

void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos)
{
  double x_center = g_width / 2.0;
  double y_center = g_height / 2.0;

  double dx = xpos - x_center;
  double dy = ypos - y_center;

  camera3DPerson->moveHoriz(-1.0 * dx * 0.01);
  camera3DPerson->moveVert(dy * 0.01); // negated becase y=0 is at the top of the screen
  //camera3DPerson->setView();

  glfwSetCursorPos(window, x_center, y_center);
}

void initPlayer(WorldGrid* gameObjects) {
    playerObject = new Player(
      &playerMesh,
      &handles,
      vec3(10, 0, 20),
      20,
      vec3(1.0, 1.0, 1.0), //scale
      vec3(1, 0, 0),
      CAMERA_SPEED,
      vec3(2.5, 2.5, 2.5),
      3,
      3
   );

   gameObjects->add(shared_ptr<GameObject>(playerObject));
}

void initGuards(WorldGrid* gameObjects) {
	vector<vec3> guardPath;
	guardPath.push_back(vec3(-5, 0, 8));
	guardPath.push_back(vec3(-9, 0, 7));
	guardPath.push_back(vec3(-11, 0, 0));
	guardPath.push_back(vec3(-9, 0, -6));
	guardPath.push_back(vec3(-5, 0, -7));

	Guard* guardObject = new Guard(
		&guardMesh,
		&handles,
		vec3(1, 1, 1),
		GUARD_SPEED,
		vec3(1.5, 1.5, 1.5),
		1,
		1,
		guardPath
	);

	gameObjects->add(shared_ptr<GameObject>(guardObject));

	/**
	ifstream file(resPath("GuardPaths.txt"), ios::in);

	while (file >> )
	*/
}

void initGround() {
    ground = new Shape(
        &handles, //model handle
        vec3(0), //position
        0, //rotation
    vec3(1.0, 1.0, 1.0), //scale
        vec3(1, 0, 0), //direction
        0, //velocity
        6, //indices
        posBufObjG, 
        norBufObjG,
        5 //material
    );
}


/*
  This assumes a grid built to [i][j], where i represents rows
  and j represents columns. 
*/
void initWalls(WorldGrid* gameObjects) {
  int levelDesign[TEST_WORLD][TEST_WORLD], tempI, tempJ, realI, realJ;
  float posX, posY;
  vec3 tempScale, tempPos;
  int testWallCount = 0;
  bool buildRight, buildDown;
  //
  char ch;
  fstream fin(resPath("LevelDesign.txt"), fstream::in);
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
  // Test print entire matrix
  for (i = 0; i < TEST_WORLD; i++) {
    cout << '\n';
    for (j = 0; j < TEST_WORLD; j++) {
      cout << levelDesign[i][j];
    }
  }
  cout << '\n';

  // Create the wall objects
  for (i = 0; i < TEST_WORLD; i++) {
    for (j = 0; j < TEST_WORLD; j++) {
      if (levelDesign[i][j]) {
        // Flags to build walls in a direction, chooses greedily
        buildDown = false;
        buildRight = false;
        // Tracks acutal size of wall object iterated over by the tempI & tempJ
        realI = realJ = 0;
        // Temporary iterators over the matrix
        tempI = i;
        tempJ = j;
        //printf("HERE j encountered is %d\n", tempJ);
        if (levelDesign[tempI][tempJ + 1] == 1 && levelDesign[tempI][tempJ + 2] == 1) {
          printf("Setting right flag\n");
          printf("Testing [%d][%d] and then checking [%d][%d]\n", tempI, tempJ + 1, tempI, tempJ + 2);
          printf("Values are %d and %d\n", levelDesign[tempI][tempJ + 1], levelDesign[tempI][tempJ + 2] == 1);
          buildRight = true;
        }
        else if (levelDesign[tempI + 1][tempJ] == 1 && levelDesign[tempI + 2][tempJ] == 1) {
          printf("Setting down flag\n");
          printf("Testing [%d][%d] and then checking [%d][%d]\n", tempI + 1, tempJ, tempI + 2, tempJ);
          printf("Values are %d and %d\n", levelDesign[tempI + 1][tempJ], levelDesign[tempI + 2][tempJ] == 1);
          buildDown = true;
        }
        // Keep building object size in a direction if possible
        while (levelDesign[tempI][tempJ] == 1) {
          if (buildRight) {
            printf("Building RIGHT with current val: %d, at [ %d ][ %d ]\n", levelDesign[tempI][tempJ], tempI, tempJ);
            levelDesign[tempI][tempJ] = 0;
            tempJ++;
            realJ++;
          }
          else if (buildDown) {
            //printf("Building DOWN with current val: %d, at [ %d ][ %d ]\n", levelDesign[tempI][tempJ], tempI, tempJ);
            levelDesign[tempI][tempJ] = 0;
            tempI++;
            realI++;
          }
        }
        tempI--;
        tempJ--;
        // Pack the temporary variables with a position and scale to create a Wall
        if      (buildRight && (tempJ - realJ) < 2) {
          tempPos = vec3((float)tempJ/2 - ((float)TEST_WORLD/2) + 2, 1, ((float)tempI) - ((float)TEST_WORLD / 2) + 2);
          tempScale = vec3(((float)realJ) / 2.0, 3.0, 1.0);
        }
        else if (buildRight && (tempJ - realJ) > 2) {
          tempPos = vec3(((float)TEST_WORLD / 2) - (float)(realJ) / 2, 1, ((float)tempI) - (float)TEST_WORLD / 2);
          tempScale = vec3(((float)realJ) / 2.0, 3.0, 1.0);
        }
        else if (buildDown  && (tempI - realI) < 2) {
          tempPos = vec3((float)tempJ - ((float)TEST_WORLD / 2) + 2, 1, ((float)tempI)/2 - ((float)TEST_WORLD / 2) + 2);
          tempScale = vec3(1.0, 3.0, ((float)realI) / 2.0);
        }
        else if (buildDown  && (tempI - realI) > 2) {
          tempPos = vec3((float)tempJ - ((float)TEST_WORLD / 2), 1, ((float)TEST_WORLD/2) - ((float)realI) / 2);
          tempScale = vec3(1.0, 3.0, ((float)realI) / 2.0);
        }

        // Make the actual Wall object and add it to gameObjects list
        gameObjects->add(shared_ptr<GameObject>(new Wall(
          &cubeMesh,
          &handles,
          tempPos,      //position
          0,            //rotation
          tempScale,    //scale
          vec3(1, 0, 0),
          0,
          vec3(1, 8, 1),//bounding box
          0,            //scanRadius
          1             //material
          )));

        /*for (int k = 0; k < TEST_WORLD; k++) {
          cout << '\n';
          for (int m = 0; m < TEST_WORLD; m++) {
            cout << levelDesign[k][m];
          }
        }
        cout << '\n';*/
        ////////// Testing only
        testWallCount++;
        //printf("Building with current val: %d, at [ %d ][ %d ]\n", levelDesign[tempI][tempJ], tempI, tempJ);
        printf("RIGHT: %d , DOWN: %d\n", buildRight, buildDown);
        printf("temps not offset: tempI: %d, tempJ: %d", tempI, tempJ);
        printf("\nCenter point of testWall: %d,  (x: %f, z: %f)\n", testWallCount, tempPos.x, tempPos.z);
        printf("With scale as (%f, %f, %f)\n", tempScale.x, tempScale.y, tempScale.z);
        //////////
      }
    }
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

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
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
    handles.installShaders(resPath(sysPath("shaders", "vert.glsl")),
                           resPath(sysPath("shaders", "frag.glsl")));

    guardMesh.loadShapes(resPath(sysPath("models", "player.obj")));
    playerMesh.loadShapes(resPath(sysPath("models", "godzilla.obj")));
    cubeMesh.loadShapes(resPath(sysPath("models", "cube.obj")));

    srand(time(NULL));

    //initialize game objects
    //vector<shared_ptr <GameObject> > gameObjects;
	//First item is always the player, followed by numGuards guards,
	//	followed by however many walls we need. -JB
    WorldGrid gameObjects(WORLD_WIDTH, WORLD_HEIGHT);

    initPlayer(&gameObjects);
    initGuards(&gameObjects);
    initWalls(&gameObjects);
    initGround();
    printf("added objects\n");
    
    //initialize the camera
    camera3DPerson = new Camera3DPerson(&handles, &gameObjects, playerObject, CAMERA_ZOOM, CAMERA_FOV,
                                        (float)g_width / (float)g_height,
                                        CAMERA_NEAR, CAMERA_FAR);
    double timeCounter = 0;
    do{
        //timer stuff
        TimeManager::Instance().CalculateFrameRate(true);
        deltaTime = TimeManager::Instance().DeltaTime;
        double currentTime = TimeManager::Instance().CurrentTime;
        timeCounter += deltaTime;

        beginDrawGL();
        // make sure these lines are in this order
        getWindowinput(window, deltaTime);
        camera3DPerson->setProjection();
        camera3DPerson->setView();
        drawGameObjects(&gameObjects, deltaTime);
        endDrawGL();

        glfwSwapBuffers(window);
        glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
        && glfwWindowShouldClose(window) == 0);

    glfwTerminate();

    return 0;
}
