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
#include "Textures/Textures.h"

// Stuff for 3D sound in windows
#if defined WIN32
#include <windows.h>
#include <conio.h>
inline void sleepSomeTime() { Sleep(100); }
#pragma comment(lib, "../dependencies/irrKlang/lib/Winx64-visualStudio/irrKlang.lib") // link with irrKlang.dll
#else
#include "../dependencies/irrKlang/examples/common/conio.h"
//#pragma comment(lib, "../dependencies/irrKlang/lib/irrKlang.lib") // link with irrKlang.dll
#endif
//#if defined __APPLE__ && __MACH__
//#include "..dependencies/irrKlang/examples/common/conio.h"
//#endif
#include "../dependencies/irrKlang/include/irrKlang.h"
using namespace irrklang;
/// Init irrKlang sound library, start the sound engine with default parameters
ISoundEngine* engine;
ISound* noseSnd;
ISound* guardTalk;
ISound* backGroundSnd;
ISound* footSndPlayr;


#define WORLD_WIDTH 300
#define WORLD_HEIGHT 300
#define TEST_WORLD 200

#define CAMERA_FOV 60
#define CAMERA_NEAR 0.1f
#define CAMERA_FAR 200.0f
#define CAMERA_ZOOM 3.0f
#define CAMERA_SPEED 10.0f

#define GUARD_SPEED 5.0f
#define BOTTOM_LEVEL 1.0f
#define MID_LEVEL 2.0f
#define TOP_LEVEL 3.0f

#define MAX_DETECT 600

GLFWwindow* window;
using namespace std;
using namespace glm;

//#pragma comment(lib, "../dependencies/irrKlang/lib/Win32-visualStudio/irrKlang.lib") // link with irrKlang.dll

vector<tinyobj::shape_t> player;
vector<tinyobj::shape_t> guard;
vector<tinyobj::material_t> materials;
vector<tinyobj::shape_t> wall;

int g_width;
int g_height;

unsigned int detectCounter = 0;
float key_speed = 0.2f; // TODO get rid of these by implementing first-person camera
float theta = 0.0f;
float phi = 0.0f;
Camera* debugCamera;
Camera3DPerson *camera3DPerson;
Player* playerObject;
vec3 oldPosition;
Handles mainShader;
Mesh guardMesh;
Mesh playerMesh;
Mesh cubeMesh;
Mesh tripleBarrelMesh;
Mesh boxStackMesh;
Mesh tableMesh;
Mesh chairMesh;
Mesh cartMesh;
Mesh rafterMesh;
Mesh textureMesh;
Mesh winMesh;
Shape *ground;
Shape *ceiling;
bool debug = false;
bool boxes = false;
DebugDraw debugDraw;

glm::vec3 g_light(0, 100, 0);

GLuint posBufObjG = 0;
GLuint norBufObjG = 0;

double deltaTime;

GLuint texture;

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
  case 0: // guards
    glUniform3f(mainShader.uMatAmb, 0.05f, 0.025f, 0.025f);
    glUniform3f(mainShader.uMatDif, 0.9f, 0.1f, 0.05f);
    glUniform3f(mainShader.uMatSpec, 0.8f, 0.2f, 0.2f);
    glUniform1f(mainShader.uMatShine, 100.0f);
    break;
  case 1: // floor
    glUniform3f(mainShader.uMatAmb, 0.13f, 0.13f, 0.14f);
    glUniform3f(mainShader.uMatDif, 0.3f, 0.3f, 0.4f);
    glUniform3f(mainShader.uMatSpec, 0.3f, 0.3f, 0.4f);
    glUniform1f(mainShader.uMatShine, 150.0f);
    break;
  case 2: // player
    glUniform3f(mainShader.uMatAmb, 0.3f, 0.3f, 0.3f);
    glUniform3f(mainShader.uMatDif, 0.9f, 0.9f, 0.9f);
    glUniform3f(mainShader.uMatSpec, 0.0f, 0.0f, 0.0f);
    glUniform1f(mainShader.uMatShine, 150.0f);
    break;
  case 3: // guard detect
    glUniform3f(mainShader.uMatAmb, 0.06f, 0.09f, 0.06f);
    glUniform3f(mainShader.uMatDif, 0.2f, 0.80f, 0.1f);
    glUniform3f(mainShader.uMatSpec, 0.8f, 1.0f, 0.8f);
    glUniform1f(mainShader.uMatShine, 4.0f);
    break;
  case 4: //big wall color
    glUniform3f(mainShader.uMatAmb, 0.2f, 0.1f, 0.0f);
    glUniform3f(mainShader.uMatDif, 0.08f, 0.0f, 0.00f);
    glUniform3f(mainShader.uMatSpec, 0.08f, 0.0f, 0.0f);
    glUniform1f(mainShader.uMatShine, 10.0f);
    break;
  case 5: // ceiling
    glUniform3f(mainShader.uMatAmb, 0.1f, 0.1f, 0.1f);
    glUniform3f(mainShader.uMatDif, 0.0f, 0.0f, 0.00f);
    glUniform3f(mainShader.uMatSpec, 1.0f, 1.0f, 1.0f);
    glUniform1f(mainShader.uMatShine, 100.0f);
    break;
  case 6: //short wall color
    glUniform3f(mainShader.uMatAmb, 0.2f, 0.2f, 0.2f);
    glUniform3f(mainShader.uMatDif, 0.08f, 0.0f, 0.00f);
    glUniform3f(mainShader.uMatSpec, 0.08f, 0.0f, 0.0f);
    glUniform1f(mainShader.uMatShine, 10.0f);
    break;
  }
}

void initGL() {
  // Set the background color
  glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
  // Enable Z-buffer test
  glEnable(GL_DEPTH_TEST);
  glPointSize(18);
  initVertexObject(&posBufObjG, &norBufObjG);
}

void getWindowinput(GLFWwindow* window, double deltaTime) {
  float forwardYVelocity = 0;
  float sideYVelocity = 0;
  bool accelerate = false;
  bool upD = false;
  bool downD = false;
  bool leftD = false;
  bool rightD = false;
  vec3 direction(0, 0, 0);
  glm::vec3 forward = camera3DPerson->getForward();
  glm::vec3 strafe = camera3DPerson->getStrafe();
  glm::vec3 up = camera3DPerson->getUp();
  oldPosition = playerObject->position;

  if (!debug) {
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      vec3 velocity = glm::vec3(strafe.x * CAMERA_SPEED * deltaTime, 
          sideYVelocity, strafe.z * CAMERA_SPEED * deltaTime);
      velocity.y = 0;
      direction += -velocity;
      glm::vec3 forward = camera3DPerson->getForward();
      playerObject->rotation = atan2f(-velocity.x, -velocity.z) * 180 / M_PI;
      //camera3DPerson->setView();
      accelerate = true;
      leftD = true;
       if (footSndPlayr->isFinished()) {
         footSndPlayr = engine->play2D("../dependencies/irrKlang/media/footstepsWalk2.wav", false, false, true);
    }
       else if (footSndPlayr->getIsPaused()) {
         footSndPlayr->setIsPaused(false);
       }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      vec3 velocity = glm::vec3(strafe.x * CAMERA_SPEED * deltaTime, 
          sideYVelocity, strafe.z * CAMERA_SPEED * deltaTime);
      velocity.y = 0;
      direction += velocity;
      glm::vec3 forward = camera3DPerson->getForward();
      playerObject->rotation = atan2f(velocity.x, velocity.z) * 180 / M_PI;
      //camera3DPerson->setView();
      accelerate = true;
      rightD = true;
       if (footSndPlayr->isFinished()) {
         footSndPlayr = engine->play2D("../dependencies/irrKlang/media/footstepsWalk2.wav", false, false, true);
    }
       else if (footSndPlayr->getIsPaused()) {
         footSndPlayr->setIsPaused(false);
       }
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      vec3 velocity = glm::vec3(forward.x * CAMERA_SPEED * deltaTime,
          forwardYVelocity, forward.z * CAMERA_SPEED * deltaTime);
      velocity.y = 0;
      direction += velocity;
      glm::vec3 forward = camera3DPerson->getForward();
      playerObject->rotation = atan2f(velocity.x, velocity.z) * 180 / M_PI;
      //camera3DPerson->setView();
      accelerate = true;
      upD = true;
       if (footSndPlayr->isFinished()) {
         footSndPlayr = engine->play2D("../dependencies/irrKlang/media/footstepsWalk2.wav", false, false, true);
     }
       else if (footSndPlayr->getIsPaused()) {
         footSndPlayr->setIsPaused(false);
       }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      vec3 velocity = glm::vec3(forward.x * CAMERA_SPEED * deltaTime,
          forwardYVelocity, forward.z * CAMERA_SPEED * deltaTime);
      velocity.y = 0;
      direction += -velocity;
      glm::vec3 forward = camera3DPerson->getForward();
      playerObject->rotation = atan2f(-velocity.x, -velocity.z) * 180 / M_PI;
      //camera3DPerson->setView();
      accelerate = true;
      downD = true;
       if (footSndPlayr->isFinished()) {
         footSndPlayr = engine->play2D("../dependencies/irrKlang/media/footstepsWalk2.wav", false, false, true);
     }
       else if (footSndPlayr->getIsPaused()) {
         footSndPlayr->setIsPaused(false);
       }
    }

    if (accelerate) {
      direction = normalize(direction);
      if ((upD && downD) || (leftD && rightD)) {
        playerObject->decelerate();
      }
      else {
        playerObject->changeDirection(direction);
        playerObject->accelerate();
        //printf("velocity: %f\n", playerObject->velocity);
      }
    }
    else {
      playerObject->decelerate();
    }
  }
  else {
    glm::vec3 view = -1.0f * debugCamera->getForward();
    glm::vec3 up = debugCamera->getUp();
    glm::vec3 strafe = debugCamera->getStrafe();
    glm::vec3 move = glm::vec3(0.0f, 0.0f, 0.0f);
    /*if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
      key_speed -= 0.1;
      }
      if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
      key_speed += 0.1;
      }*/
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      move = -1.0f * key_speed * strafe;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      move = -1.0f * key_speed * strafe;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      move = key_speed * strafe;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      move = key_speed * strafe;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      move = -1.0f * key_speed * view;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      move = -1.0f * key_speed * view;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      move = key_speed * view;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      move = key_speed * view;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
      move = key_speed * glm::vec3(0, -1, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
      move = key_speed * glm::vec3(0, 1, 0);
    }
    debugCamera->eye += move;
    debugCamera->lookat += move;
  }
}

void drawGameObjects(WorldGrid* gameObjects, float time) {
  SetMaterial(ground->material);
  ground->draw();
  SetMaterial(ceiling->material);
  ceiling->draw();
  Guard *guard;
  // draw
  vector<shared_ptr<GameObject>> drawList = camera3DPerson->getUnculled(gameObjects);
  for (int i = 0; i < drawList.size(); i++) {
    SetMaterial(drawList[i]->material);
    drawList[i]->draw();
  }

  // collide
  for (int i = 0; i < gameObjects->list.size(); i++) {
    gameObjects->list[i]->move(time);
    vector<shared_ptr<GameObject>> proximity = 
      gameObjects->getCloseObjects(gameObjects->list[i]);

    //all objects
    for (int j = 0; j < proximity.size(); j++) {
      if (gameObjects->list[i].get() != proximity[j].get()) {
        if (gameObjects->list[i]->collide(proximity[j].get())) {
          //do some stuff
        } 
      }
    }

    //players
    if (dynamic_cast<Player*>(gameObjects->list[i].get())) {
      engine->setListenerPosition(vec3df(gameObjects->list[i].get()->position.x, gameObjects->list[i].get()->position.y, gameObjects->list[i].get()->position.z),
        vec3df(gameObjects->list[i].get()->direction.x, gameObjects->list[i].get()->direction.y, gameObjects->list[i].get()->direction.z));
    for (int j = 0; j < gameObjects->wallList.size(); j++) {
      if (gameObjects->list[i]->collide(gameObjects->wallList[j].get())) {
          // Example of event based sound, just for fun
            if (noseSnd->isFinished()) {
              noseSnd = engine->play2D("../dependencies/irrKlang/media/ow_my_nose.wav", false, false, true);
          }
          else if (noseSnd->getIsPaused()) {
            noseSnd->setIsPaused(false);
          }
      }
    }

    //guards
    if (guard = dynamic_cast<Guard*>(gameObjects->list[i].get())) {
      if (guard->detect(playerObject)) {
        cout << "Detection: " << ++detectCounter << " out of " << MAX_DETECT << endl;
        if (detectCounter >= MAX_DETECT) {
          // TODO lose
#ifndef DEBUG
          cout << "You lose! Not sneaky enough!" << endl;
          exit(0);
#endif
        }
      }
    }

        for (int j = 0; j < proximity.size(); j++) {
            if (gameObjects->list[i] != proximity[j]) {
                if (gameObjects->list[i]->collide(proximity[j].get())) {
                  //do some shit
            if (guardTalk->isFinished()) {
                    guardTalk = engine->play3D("../dependencies/irrKlang/media/killing_to_me.wav", 
                      vec3df(proximity[j].get()->position.x, proximity[j].get()->position.y, proximity[j].get()->position.z), false, false, true);
                  }
                  else if (guardTalk->getIsPaused()) {
                    guardTalk = engine->play3D("../dependencies/irrKlang/media/killing_to_me.wav", 
                      vec3df(proximity[j].get()->position.x, proximity[j].get()->position.y, proximity[j].get()->position.z), false, true, true);
                    guardTalk->setIsPaused(false);
            }
                }
            }
        }
    }
    /*for (int i = 0; i < gameObjects->wallList.size(); i++) {
      SetMaterial(gameObjects->wallList[i]->material);
      gameObjects->wallList[i]->draw();
  }*/

  gameObjects->update();
}
}

void beginDrawGL() {
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use our GLSL program
  glUseProgram(mainShader.prog);
  glUniform3f(mainShader.uLightPos, g_light.x, g_light.y, g_light.z);
  glUniform3f(mainShader.uCamPos, camera3DPerson->eye.x,
      camera3DPerson->eye.y, camera3DPerson->eye.z);
  GLSL::enableVertexAttribArray(mainShader.aPosition);
  GLSL::enableVertexAttribArray(mainShader.aNormal);
}

void endDrawGL() {
  GLSL::disableVertexAttribArray(mainShader.aPosition);
  GLSL::disableVertexAttribArray(mainShader.aNormal);
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
    debug = !debug;
  }
  if (key == GLFW_KEY_B && action == GLFW_PRESS) {
    boxes = !boxes;
  }
  if (!debug) {
    if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera3DPerson->zoom *= 0.9;
    }
    if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera3DPerson->zoom *= 1.1;
    }
  }
  else {
    if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      key_speed *= 0.9;
    }
    if (key == GLFW_KEY_P && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      key_speed *= 1.1;
    }
  }

  if (key == GLFW_KEY_LEFT_SHIFT) {
    if (action == GLFW_PRESS) {
      playerObject->SetMotion(RUN);
    }
    else if (action == GLFW_RELEASE) {
      playerObject->SetMotion(WALK);
    }
  }
  if (key == GLFW_KEY_LEFT_CONTROL) {
    if (action == GLFW_PRESS) {
      playerObject->SetMotion(CROUCH);
      playerObject->crouch = true;
    }
    else if (action == GLFW_RELEASE) {
      playerObject->SetMotion(WALK);
      playerObject->crouch = false;
    }

  }
}

void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos)
{
  double x_center = g_width / 2.0;
  double y_center = g_height / 2.0;

  double dx = xpos - x_center;
  double dy = ypos - y_center;

  if (!debug) {
    camera3DPerson->moveHoriz(-1.0 * dx * 0.01);
    camera3DPerson->moveVert(dy * 0.01); // negated becase y=0 is at the top of the screen
  }
  else {
    // TODO implement first person camera class 
    double max_vert_angle = 85;
    double cursor_speed = 0.3;

    theta -= dx * cursor_speed;
    // note that ypos is measured from the top of the screen, so
    // an increase in ypos means moving the mouse down the y axis
    if ((phi < max_vert_angle && dy > 0) || (phi > -1.0 * max_vert_angle && dy < 0)) {
      phi -= dy * cursor_speed;
    }
    debugCamera->lookat.x = debugCamera->eye.x + cos(phi * M_PI / 180) * cos(theta * M_PI / 180);
    debugCamera->lookat.y = debugCamera->eye.y + sin(phi * M_PI / 180);
    debugCamera->lookat.z = debugCamera->eye.z + cos(phi * M_PI / 180) * sin(-1.0 * theta * M_PI / 180);
  }

  glfwSetCursorPos(window, x_center, y_center);
}

void initObjects(WorldGrid* gameObjects) {
  int levelDesign[TEST_WORLD][TEST_WORLD];

  char ch;
  fstream fin(resPath("LevelDesignFull.txt"), fstream::in);
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

  //////////// Create the wall objects
  for (i = 0; i < TEST_WORLD; i++) {
    for (j = 0; j < TEST_WORLD; j++) {
      switch(levelDesign[i][j]) {
        case 3: //barrels
          gameObjects->add(shared_ptr<GameObject>(new GameObject(
          &tripleBarrelMesh,
          &mainShader,
          vec3(i - (TEST_WORLD/2), 1, j - (TEST_WORLD/2)),
          getRand(0, 360), 
          vec3(2.5, 2.5, 2.5),
          vec3(1.0, 0, 0),
          0,
          vec3(4.5, 4, 4.5),
          1,
          0,
          false
          )));
          break;
        case 4: //stack of boxes
          gameObjects->add(shared_ptr<GameObject>(new GameObject(
          &boxStackMesh,
          &mainShader,
          vec3(i - (TEST_WORLD/2), 1, j - (TEST_WORLD/2)),
          getRand(0, 360), 
          vec3(4, 2, 4),
          vec3(1.0, 0.0, 0.0),
          0,
          vec3(3.0, 5, 3.0),
          1,
          0,
          false
          )));
          break;
        case 5: //table
          gameObjects->add(shared_ptr<GameObject>(new GameObject(
          &tableMesh,
          &mainShader,
          vec3(i - (TEST_WORLD/2), -0.50, j - (TEST_WORLD/2)),
          0, 
          vec3(1.5, 1, 1.5),
          vec3(1.0, 0.0, 0.0),
          0,
          vec3(2.8, 1.5, 1.4),
          1,
          0,
          true
          )));
          break;
        case 6: //chair
          gameObjects->add(shared_ptr<GameObject>(new GameObject(
          &chairMesh,
          &mainShader,
          vec3(i - (TEST_WORLD/2), 0, j - (TEST_WORLD/2)),
          getRand(0, 360), 
          vec3(1, 1, 1),
          vec3(1.0, 0.0, 0.0),
          0,
          vec3(1.5, 2, 1.5),
          1,
          0,
          true
          )));
          break;
        case 7: //cart
          gameObjects->add(shared_ptr<GameObject>(new GameObject(
          &cartMesh,
          &mainShader,
          vec3(i - (TEST_WORLD/2), -0.25, j - (TEST_WORLD/2)),
          getRand(0, 360), 
          vec3(1, 1, 1),
          vec3(1.0, 0.0, 0.0),
          0,
          vec3(2.2, 2, 1.2),
          1,
          0,
          true
          )));
          break;
        case 8: //rafter
          gameObjects->add(shared_ptr<GameObject>(new GameObject(
          &rafterMesh,
          &mainShader,
          vec3(i - (TEST_WORLD/2), 16, j - (TEST_WORLD/2)),
          90, 
          vec3(24, 15, 24),
          vec3(1.0, 0.0, 0.0),
          0,
          vec3(44, 5.0, 1.0),
          1,
          6,
          false
          )));
          break;
        case 9: //flag
          gameObjects->add(shared_ptr<GameObject>(new GameObject(
          &winMesh,
          &mainShader,
          vec3(i - (TEST_WORLD/2), 3.2, j - (TEST_WORLD/2)),
          0, 
          vec3(4, 6, 4),
          vec3(1.0, 0.0, 0.0),
          0,
          vec3(1, 10, 1),
          1,
          3,
          false
          )));
          break;
        default:
          break;
      }
    }
  }
}

void initPlayer(WorldGrid* gameObjects) {
  playerObject = new Player(
      &playerMesh,
      &mainShader,
      vec3(0, 0, 0),
      20,
      vec3(1.0, 1.0, 1.0), //scale
      vec3(1, 0, 0),
      vec3(1.0, 2.0, 1.0),
      1,
      2
      );

  gameObjects->add(shared_ptr<GameObject>(playerObject));
}

void initGuards(WorldGrid* gameObjects) {
  vector<PathNode> guardPath;
  FILE *file = fopen(resPath("GuardPaths.txt").data(), "r");
  char line[100];
  float x, y, z, dur;
  char smartTurn, endTurnDir;
  int numNodes;

  while (fgets(line, 100, file)) {
    if (line[0] == 'G') { // build new guard
      guardPath.clear();
      fgets(line, 100, file); // guard settings, ignored for now
      fscanf(file, "%s %d", line, &numNodes); // read number of nodes in path

      for (int i = 0; i < numNodes; i++) { // read in numNodes nodes
        fscanf(file, "%f %f %f %c %f %c", &x, &y, &z, &smartTurn, &dur, &endTurnDir);
        printf("NODE: %f %f %f %c %f %c\n", x, y, z, smartTurn, dur, endTurnDir);
        guardPath.push_back(PathNode(vec3(x, y, z), smartTurn == 'y', dur, endTurnDir == 'r', endTurnDir != 'x'));
      }

      Guard* guardObject = new Guard(
          &guardMesh,
          &mainShader,
          vec3(1, 1, 1),
          GUARD_SPEED,
          vec3(1.0, 2.0, 1.0),
          1,
          0,
          guardPath
          );
      gameObjects->add(shared_ptr<GameObject>(guardObject));
    }
  }
}

void initGround() {
  ground = new Shape(
      &mainShader, //model handle
      vec3(0), //position
      0, //rotation
      vec3(5, 1, 5), //scale
      vec3(1, 0, 0), //direction
      0, //velocity
      6, //indices
      posBufObjG, 
      norBufObjG,
      1 //material
      );
}

void initCeiling() {
  ceiling = new Shape(
      &mainShader, //model handle
      vec3(0, 20, 0), //position
      0, //rotation
      vec3(5, 1, 5), //scale
      vec3(1, 0, 0), //direction
      0, //velocity
      6, //indices
      posBufObjG, 
      norBufObjG,
      5 //material
      );
}

void initWalls(WorldGrid* gameObjects) {
	int levelDesign[TEST_WORLD][TEST_WORLD], tempI, tempJ, realI, realJ;
	float posX, posY;
	vec3 tempScale, tempPos, tempBBox;
	int testWallCount = 0;
  bool shortWall = false;

	char ch;
	fstream fin(resPath("LevelDesignFull.txt"), fstream::in);
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

	///////// Test print entire matrix
//	for (i = 0; i < TEST_WORLD; i++) {
//		cout << '\n';
//		for (j = 0; j < TEST_WORLD; j++) {
//			cout << levelDesign[i][j];
//    }
//}
//	cout << '\n';

	vec2 start, end;
	//////////// Create the wall objects
	for (i = 0; i < TEST_WORLD; i++) {
		for (j = 0; j < TEST_WORLD; j++) {
			if (levelDesign[i][j] == 1 || levelDesign[i][j] == 2) {
        if (levelDesign[i][j] == 2) {
          shortWall = true;
        }
				start = vec2(i, j);
				int i1 = i, j1 = j;
				// build wall depending on direction it is oriented
				if (i1 != TEST_WORLD - 1 && levelDesign[i1 + 1][j1] == 1 ||
          i1 != TEST_WORLD - 1 && levelDesign[i1 + 1][j1] == 2) { // going right
					while (i1 < TEST_WORLD && levelDesign[i1][j1] == 1 ||
          i1 < TEST_WORLD && levelDesign[i1][j1] == 2) {
						levelDesign[i1][j1] = 0;
						i1++;
					}
					end = vec2(i1 - 1, j1);
				}
				else if (j1 != TEST_WORLD - 1 && levelDesign[i1][j1 + 1] == 1 ||
          j1 != TEST_WORLD - 1 && levelDesign[i1][j1 + 1] == 2) { // going down
					while (j1 < TEST_WORLD && levelDesign[i1][j1] == 1 ||
            j1 < TEST_WORLD && levelDesign[i1][j1] == 2) {
						levelDesign[i1][j1] = 0;
						j1++;
					}
					end = vec2(i1, j1 - 1);
				}
				else { // 1x1 wall
					end = start;
				}

				vec2 center((start.x + end.x - TEST_WORLD + 1) / 2, (start.y + end.y - TEST_WORLD + 1) / 2);
				vec2 dims(abs(end.x - start.x) + 1, abs(end.y - start.y) + 1);

				// Make the actual Wall object and add it to gameObjects list
        if (shortWall) {
          gameObjects->add(shared_ptr<GameObject>(new Wall(
            &cubeMesh,
            &mainShader,
            vec3(center.x, 0, center.y),      //position
            0,            //rotation
            vec3(dims.x / 2, 1, dims.y / 2),    //scale
            vec3(1, 0, 0),  //direction
            0,
            vec3(dims.x, 1, dims.y),     //dimensions
            0,            //scanRadius
            6             //material
            )));
            shortWall = false;     
        }
        else {
				gameObjects->add(shared_ptr<GameObject>(new Wall(
					&cubeMesh,
					&mainShader,
					vec3(center.x, 9, center.y),      //position
					0,            //rotation
					vec3(dims.x / 2, 10, dims.y / 2),    //scale
            vec3(1, 0, 0),  //direction
					0,
					vec3(dims.x, 20, dims.y),     //dimensions
					0,            //scanRadius
					4             //material
					)));
        }
				testWallCount++;
				printf("\nCenter point of testWall: %d,  (x: %f, z: %f)\n", testWallCount, tempPos.x, tempPos.z);
			}
		}
	}
}

//void initTexture() {
//  float CubePos[] = {
//    //-worldSize, -3.0, -worldSize, //back face 5 verts :0 - clockwise 
//    //-worldSize, worldSize, -worldSize,
//    //worldSize, worldSize, -worldSize,
//    //worldSize, -3.0, -worldSize,
//
//    //worldSize, -3.0, worldSize, //right face 5 verts :4
//    //worldSize, worldSize, worldSize,
//    //worldSize, worldSize, -worldSize,
//    //worldSize, -3.0, -worldSize,
//
//    //-worldSize, -3.0, worldSize, //front face 4 verts :8
//    //-worldSize, worldSize, worldSize,
//    //worldSize, worldSize, worldSize,
//    //worldSize, -3.0, worldSize,
//
//    //-worldSize, -3.0, -worldSize, //left face 4 verts :12
//    //-worldSize, worldSize, -worldSize,
//    //-worldSize, worldSize, worldSize,
//    //-worldSize, -3.0, worldSize
//  };
//
//  static GLfloat CubeTex[] = {
//    .75, 0, // back 
//    .75, 1,
//    .5, 1,
//    .5, 0,
//
//    .25, 0, //right 
//    .25, 1,
//    .5, 1,
//    .5, 0,
//
//    0, 0, //front 
//    0, 1,
//    .25, 1,
//    .25, 0,
//
//    .75, 0, // left 
//    .75, 1,
//    1, 1,
//    1, 0
//  };
//
//  unsigned short idx[] = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12 };
//
//  //g_CiboLen = 24;
//  //glGenBuffers(1, &CubeBuffObj);
//  //glBindBuffer(GL_ARRAY_BUFFER, CubeBuffObj);
//  //glBufferData(GL_ARRAY_BUFFER, sizeof(CubePos), CubePos, GL_STATIC_DRAW);
//
//  //glGenBuffers(1, &CIndxBuffObj);
//  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CIndxBuffObj);
//  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
//
//  //glGenBuffers(1, &TexBuffObj);
//  //glBindBuffer(GL_ARRAY_BUFFER, TexBuffObj);
//  //glBufferData(GL_ARRAY_BUFFER, sizeof(CubeTex), CubeTex, GL_STATIC_DRAW);
//}

int main(int argc, char **argv)
{
     // irrKlang init
     engine = createIrrKlangDevice();
     if (!engine) {
       return 0; // error starting up the engine
     }
     // Play something on loop for background music
     backGroundSnd = engine->play2D("../dependencies/irrKlang/media/red_sky_at_night.wav", true, true, true);
     backGroundSnd->setVolume(.2);
     backGroundSnd->setIsPaused(false);
     noseSnd = engine->play2D("../dependencies/irrKlang/media/ow_my_nose.wav", false, true, true);
     footSndPlayr = engine->play2D("../dependencies/irrKlang/media/footstepsWalk2.wav", false, true, true);
     guardTalk = engine->play3D("../dependencies/irrKlang/media/killing_to_me.wav", vec3df(0, 0, 0), false, true, true);
     guardTalk->setVolume(1);
     engine->play2D("../dependencies/irrKlang/media/killing_to_me.wav", false, false, true);

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
  debugDraw.handles.installShaders(resPath(sysPath("shaders", "vert_debug.glsl")), resPath(sysPath("shaders", "frag_debug.glsl")));
  mainShader.installShaders(resPath(sysPath("shaders", "vert.glsl")), resPath(sysPath("shaders", "frag.glsl")));
  //mainShader.installShaders(resPath(sysPath("shaders", "vert_nor.glsl")), resPath(sysPath("shaders", "frag_nor.glsl")));

  guardMesh.loadShapes(resPath(sysPath("models", "player.obj")));
  playerMesh.loadShapes(resPath(sysPath("models", "player.obj")));
  cubeMesh.loadShapes(resPath(sysPath("models", "cube.obj")));
  tripleBarrelMesh.loadShapes(resPath(sysPath("models", "tripleBarrel.obj")));
  boxStackMesh.loadShapes(resPath(sysPath("models", "boxStack.obj")));
  tableMesh.loadShapes(resPath(sysPath("models", "table.obj")));
  chairMesh.loadShapes(resPath(sysPath("models", "chair.obj")));
  cartMesh.loadShapes(resPath(sysPath("models", "cart.obj")));
  rafterMesh.loadShapes(resPath(sysPath("models", "rafter.obj")));
  winMesh.loadShapes(resPath(sysPath("models", "flag.obj")));

  srand(time(NULL));

  //initialize game objects
  //vector<shared_ptr <GameObject> > gameObjects;
  //First item is always the player, followed by numGuards guards,
  //	followed by however many walls we need. -JB
  WorldGrid gameObjects(WORLD_WIDTH, WORLD_HEIGHT);

  initPlayer(&gameObjects);
  initGuards(&gameObjects);
  initObjects(&gameObjects);
  initWalls(&gameObjects);
  initGround();
  initCeiling();
      
    //initialize the camera
  camera3DPerson = new Camera3DPerson(&mainShader, &gameObjects, playerObject, CAMERA_ZOOM, CAMERA_FOV,
      (float)g_width / (float)g_height,
      CAMERA_NEAR, CAMERA_FAR);
  camera3DPerson->debug = &debugDraw;
  // debug camera
  debugCamera = new Camera(&mainShader,
      glm::vec3(0.0f, 0.0f, 1.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      CAMERA_FOV,
      (float)g_width / (float)g_height,
      CAMERA_NEAR,
      CAMERA_FAR);
  double timeCounter = 0;


  // Textures load
  Textures* textureObj = new Textures();
  textureObj->LoadTexture((char *)"../resources/Textures/sunset.bmp", 0);
  //texture = textureObj->LoadTexture("../resources/Textures/sunset.bmp");

  do{
    //timer stuff
    TimeManager::Instance().CalculateFrameRate(true);
    deltaTime = TimeManager::Instance().DeltaTime;
    double currentTime = TimeManager::Instance().CurrentTime;
    timeCounter += deltaTime;

    beginDrawGL();
    // make sure these lines are in this order
    getWindowinput(window, deltaTime);
    if (!debug) {
      camera3DPerson->setProjection();
      camera3DPerson->setView();
    }
    else {
      debugCamera->setProjection();
      debugCamera->setView();
    }

    // draw debug
    if (debug || boxes) {
      if (debug) {
        camera3DPerson->getView();
        camera3DPerson->getProjection();
        debugDraw.view = debugCamera->getView();
        debugDraw.projection = debugCamera->getProjection();
      }
      else {
        debugDraw.view = camera3DPerson->getView();
        debugDraw.projection = camera3DPerson->getProjection();
      }

      vector<shared_ptr<GameObject>> objs = gameObjects.list;
      for (auto objIter = objs.begin(); objIter != objs.end(); ++objIter) {
        debugDraw.addBox((*objIter)->position, (*objIter)->dimensions, glm::vec3(0.7f, 0.1f, 1.0f), false);
      }

      vector<shared_ptr<GameObject>> walls = gameObjects.wallList;
      for (auto objIter = walls.begin(); objIter != walls.end(); ++objIter) {
        debugDraw.addBox((*objIter)->position, (*objIter)->dimensions, glm::vec3(0.7f, 0.1f, 1.0f), false);
      }
    }

    drawGameObjects(&gameObjects, deltaTime);
    endDrawGL();
    if (debug || boxes) {
      debugDraw.drawAll();
      debugDraw.clear();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
      && glfwWindowShouldClose(window) == 0);

  glfwTerminate();
     backGroundSnd->drop();
     noseSnd->drop();
     footSndPlayr->drop();
     guardTalk->drop();
     engine->drop();
  return 0;
}
