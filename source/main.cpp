#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <cassert>
#define _USE_MATH_DEFINES
//#define DEBUG 1
#include <cmath>
#include <math.h>
#include <time.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, txrans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr
#include <memory>
//#include ""

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
#include "GameObject/ShadowMapPass1Handles.h"
#include "GameObject/ShadowMapPass2Handles.h"
#include "GameObject/Mesh.h"
#include "GameObject/WinCondition.h"
#include "Camera/Camera.h"
#include "Camera/Camera3DPerson.h"
#include "Camera/DetectionCamera.h"
#include "Path/Path.h"
#include "WorldGrid/WorldGrid.h"
#include "MySound/MySound.h"
#include "Textures/Textures.h"
#include "DetectionTracker/DetectionTracker.h"
#include "GameObject/VisualMeter.h"

//#include "GuardPath/PathNode.h"
//#define DEBUG

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

#define MAX_DETECT 400

GLFWwindow* window;
using namespace std;
using namespace glm;

vector<tinyobj::shape_t> player;
vector<tinyobj::shape_t> guard;
vector<tinyobj::material_t> materials;
vector<tinyobj::shape_t> wall;

int g_width;
int g_height;

float detectCounter = 0;
float key_speed = 0.2f; // TODO get rid of these by implementing first-person camera
float theta = 0.0f;
float phi = 0.0f;
Camera* debugCamera;
Camera3DPerson *camera3DPerson;
Player* playerObject;
vec3 oldPosition;
Pass1Handles pass1Handles;
Pass2Handles pass2Handles;
Mesh guardMesh;
Mesh playerMesh;
Mesh cubeMesh;
Mesh tripleBarrelMesh;
Mesh boxStackMesh;
Mesh tableMesh;
Mesh chairMesh;
Mesh cartMesh;
Mesh rafterMesh;
Mesh winMesh;
Shape *ground;
Shape *ceiling;
bool debug = false;
bool boxes = false;
DebugDraw *debugDraw;
DetectionCamera *detectCam;
MySound *soundObj;
DetectionTracker *detecTrack;

glm::vec3 g_light(0, 10, -5);

GLuint posBufObjG = 0;
GLuint norBufObjG = 0;
GLuint frameBufObj = 0;
GLuint shadowMap = 0;

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

/* helper function to make sure your matrix handle is correct */
inline void safe_glUniformMatrix4fv(const GLint handle, const GLfloat data[]) {
  if (handle >= 0)
    glUniformMatrix4fv(handle, 1, GL_FALSE, data);
}

void SetMaterial(int i) {
  switch (i) {
  case 0: // guards
    glUniform3f(pass2Handles.uMatAmb, 0.05f, 0.025f, 0.025f);
    glUniform3f(pass2Handles.uMatDif, 0.9f, 0.1f, 0.05f);
    glUniform3f(pass2Handles.uMatSpec, 0.8f, 0.2f, 0.2f);
    glUniform1f(pass2Handles.uMatShine, 100.0f);
    glUniform1f(pass2Handles.uMatRoughness, 0.8);
    glUniform1f(pass2Handles.uFresReflectance, 2);
    break;
  case 1: // floor
    glUniform3f(pass2Handles.uMatAmb, 0.13f, 0.13f, 0.14f);
    glUniform3f(pass2Handles.uMatDif, 0.3f, 0.3f, 0.4f);
    glUniform3f(pass2Handles.uMatSpec, 0.3f, 0.3f, 0.4f);
    glUniform1f(pass2Handles.uMatShine, 150.0f);
    glUniform1f(pass2Handles.uMatRoughness, 0.8);
    glUniform1f(pass2Handles.uFresReflectance, 2);
    break;
  case 2: // player
    glUniform3f(pass2Handles.uMatAmb, 0.3f, 0.3f, 0.3f);
    glUniform3f(pass2Handles.uMatDif, 0.9f, 0.9f, 0.9f);
    glUniform3f(pass2Handles.uMatSpec, 0.0f, 0.0f, 0.0f);
    glUniform1f(pass2Handles.uMatShine, 150.0f);
    glUniform1f(pass2Handles.uMatRoughness, 0.8);
    glUniform1f(pass2Handles.uFresReflectance, 2);
    break;
  case 3: // guard detect
    glUniform3f(pass2Handles.uMatAmb, 0.06f, 0.09f, 0.06f);
    glUniform3f(pass2Handles.uMatDif, 0.2f, 0.80f, 0.1f);
    glUniform3f(pass2Handles.uMatSpec, 0.8f, 1.0f, 0.8f);
    glUniform1f(pass2Handles.uMatShine, 4.0f);
    glUniform1f(pass2Handles.uMatRoughness, 0.5);
    glUniform1f(pass2Handles.uFresReflectance, 0.5);
    break;
  case 4: //big wall color
    glUniform3f(pass2Handles.uMatAmb, 0.2f, 0.1f, 0.0f);
    glUniform3f(pass2Handles.uMatDif, 0.08f, 0.0f, 0.00f);
    glUniform3f(pass2Handles.uMatSpec, 0.08f, 0.0f, 0.0f);
    glUniform1f(pass2Handles.uMatShine, 10.0f);
    glUniform1f(pass2Handles.uMatRoughness, 1);
    glUniform1f(pass2Handles.uFresReflectance, .3);
    break;
  case 5: // ceiling
    glUniform3f(pass2Handles.uMatAmb, 0.1f, 0.1f, 0.1f);
    glUniform3f(pass2Handles.uMatDif, 0.0f, 0.0f, 0.00f);
    glUniform3f(pass2Handles.uMatSpec, 1.0f, 1.0f, 1.0f);
    glUniform1f(pass2Handles.uMatShine, 100.0f);
    glUniform1f(pass2Handles.uMatRoughness, 0.8);
    glUniform1f(pass2Handles.uFresReflectance, 2);
    break;
  case 6: //short wall color
    glUniform3f(pass2Handles.uMatAmb, 0.2f, 0.2f, 0.2f);
    glUniform3f(pass2Handles.uMatDif, 0.08f, 0.0f, 0.00f);
    glUniform3f(pass2Handles.uMatSpec, 0.08f, 0.0f, 0.0f);
    glUniform1f(pass2Handles.uMatShine, 10.0f);
    glUniform1f(pass2Handles.uMatRoughness, 0.5);
    glUniform1f(pass2Handles.uFresReflectance, 0.5);
    break;
  case 7: //box stack
    glUniform3f(pass2Handles.uMatAmb, 0.39f, 0.20f, 0.1f);
    glUniform3f(pass2Handles.uMatDif, 0.28f, 0.1f, 0.00f);
    glUniform3f(pass2Handles.uMatSpec, 0.08f, 0.0f, 0.0f);
    glUniform1f(pass2Handles.uMatShine, 10.0f);
    glUniform1f(pass2Handles.uMatRoughness, 0.8);
    glUniform1f(pass2Handles.uFresReflectance, 2);
    break;
  case 8: //chairs
    glUniform3f(pass2Handles.uMatAmb, 0.05f, 0.1f, 0.25f);
    glUniform3f(pass2Handles.uMatDif, 0.1f, 0.15f, 0.2f);
    glUniform3f(pass2Handles.uMatSpec, 0.08f, 0.0f, 0.1f);
    glUniform1f(pass2Handles.uMatShine, 10.0f);
    glUniform1f(pass2Handles.uMatRoughness, 0.1);
    glUniform1f(pass2Handles.uFresReflectance, 0.5);
    break;
  }
}

void SetModel(GLint handle, vec3 trans, float rot, vec3 sc) {
  glm::mat4 Trans = glm::translate(glm::mat4(1.0f), trans);
  glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0));
  glm::mat4 Sc = glm::scale(glm::mat4(1.0f), sc);
  glm::mat4 com = Trans*RotateY*Sc;
  if (handle >= 0)
    glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(com));
}

void SetDepthMVP(bool pass1, vec3 position, float rot, vec3 scale) {

  glm::vec3 lightInv = g_light;
  glm::mat4 depthProjMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
  glm::mat4 depthViewMatrix = glm::lookAt(lightInv, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0, 1, 0));
  glm::mat4 depthModelMatrix = glm::translate(glm::mat4(1.0f), position) * 
    glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0f), scale);
  //glm::mat4 depthModelMatrix = glm::mat4(1.0f);
  glm::mat4 depthMVP = depthProjMatrix * depthViewMatrix * depthModelMatrix;

  glm::mat4 biasMatrix(
		       0.5, 0.0, 0.0, 0.0,
		       0.0, 0.5, 0.0, 0.0,
		       0.0, 0.0, 0.5, 0.0,
		       0.5, 0.5, 0.5, 1.0
		       );

  glm::mat4 depthBiasMVP = biasMatrix*depthMVP;

  
  pass1 ? safe_glUniformMatrix4fv(pass1Handles.uDepthMVP, glm::value_ptr(depthMVP)) : 
    safe_glUniformMatrix4fv(pass2Handles.uDepthMVP, glm::value_ptr(depthBiasMVP));

  //cerr << glGetError() << endl;
  assert(glGetError() == GL_NO_ERROR);

}

void initFramebuffer() {
  glGenFramebuffersEXT(1, &frameBufObj);
  assert(frameBufObj > 0);
  glBindFramebufferEXT(GL_FRAMEBUFFER, frameBufObj);
  assert(glGetError() == GL_NO_ERROR);

  glGenTextures(1, &shadowMap);
  assert(shadowMap > 0);
  glBindTexture(GL_TEXTURE_2D, shadowMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1080, 720, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  assert(glGetError() == GL_NO_ERROR);
  checkGLError();

  //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    //cerr << "Frame buffer not ok!" << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
  //}

  // Unbind the arrays                                                                                              
  glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
  assert(glGetError() == GL_NO_ERROR);
}

void initGL() {
  // Set the background color
  glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
  // Enable Z-buffer test
  glEnable(GL_DEPTH_TEST);
  glPointSize(18);
  initVertexObject(&posBufObjG, &norBufObjG);
  initFramebuffer();
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
      accelerate = true;
      leftD = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      vec3 velocity = glm::vec3(strafe.x * CAMERA_SPEED * deltaTime, 
          sideYVelocity, strafe.z * CAMERA_SPEED * deltaTime);
      velocity.y = 0;
      direction += velocity;
      glm::vec3 forward = camera3DPerson->getForward();
      playerObject->rotation = atan2f(velocity.x, velocity.z) * 180 / M_PI;
      accelerate = true;
      rightD = true;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      vec3 velocity = glm::vec3(forward.x * CAMERA_SPEED * deltaTime,
          forwardYVelocity, forward.z * CAMERA_SPEED * deltaTime);
      velocity.y = 0;
      direction += velocity;
      glm::vec3 forward = camera3DPerson->getForward();
      playerObject->rotation = atan2f(velocity.x, velocity.z) * 180 / M_PI;
      accelerate = true;
      upD = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      vec3 velocity = glm::vec3(forward.x * CAMERA_SPEED * deltaTime,
          forwardYVelocity, forward.z * CAMERA_SPEED * deltaTime);
      velocity.y = 0;
      direction += -velocity;
      glm::vec3 forward = camera3DPerson->getForward();
      playerObject->rotation = atan2f(-velocity.x, -velocity.z) * 180 / M_PI;
      accelerate = true;
      downD = true;
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

void drawPass1(WorldGrid* gameObjects) {
  Guard *guard;
  // draw
  //vector<shared_ptr<GameObject>> drawList = camera3DPerson->getUnculled(gameObjects);
  //SetDepthMVP(true, ground->position, ground->rotation, ground->scale);
  //pass1Handles.draw(ground);
  vector<shared_ptr<GameObject>> drawList = gameObjects->list;
  vector<shared_ptr<GameObject>> walls = gameObjects->wallList;
  drawList.insert(drawList.end(), walls.begin(), walls.end());
  for (int i = 0; i < drawList.size(); i++) {
    SetDepthMVP(true, drawList[i]->position, drawList[i]->rotation, drawList[i]->scale);
    pass1Handles.draw(drawList[i].get());
    //drawList[i]->draw();
  }

  gameObjects->update();
}

void drawGameObjects(WorldGrid* gameObjects, float time) {
  SetMaterial(ground->material);
  SetDepthMVP(false, ground->position, ground->rotation, ground->scale);
  SetModel(pass2Handles.uModelMatrix, ground->position, ground->rotation, ground->scale);
  pass2Handles.draw(ground);
  //ground->draw();
  SetMaterial(ceiling->material);
  SetDepthMVP(false, ceiling->position, ceiling->rotation, ceiling->scale);
  SetModel(pass2Handles.uModelMatrix, ceiling->position, ceiling->rotation, ceiling->scale);
  pass2Handles.draw(ceiling);
  //ceiling->draw();
  Guard *guard;
  // draw
  vector<shared_ptr<GameObject>> drawList = camera3DPerson->getUnculled(gameObjects);
  for (int i = 0; i < drawList.size(); i++) {
    SetMaterial(drawList[i]->material);
    SetDepthMVP(false, drawList[i]->position, drawList[i]->rotation, drawList[i]->scale);
    SetModel(pass2Handles.uModelMatrix, drawList[i]->position, drawList[i]->rotation, drawList[i]->scale);
    pass2Handles.draw(drawList[i].get());
    //drawList[i]->draw();
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

    if (dynamic_cast<Player *>(gameObjects->list[i].get())) {
      soundObj->setListenerPos(gameObjects->list[i].get()->position, gameObjects->list[i].get()->direction);
      detecTrack->updateSndDetect(dynamic_cast<Player *>(gameObjects->list[i].get()));
      //detecTrack->updateVisMeter(dynamic_cast<Player *>(gameObjects->list[i].get()));
      for (int j = 0; j < gameObjects->wallList.size(); j++) {
        if (gameObjects->list[i]->collide(gameObjects->wallList[j].get())) {
          soundObj->noseSnd = soundObj->startSound(soundObj->noseSnd, "../dependencies/irrKlang/media/ow_my_nose.wav");
        }
      }
    }

    //guards
    if (guard = dynamic_cast<Guard*>(gameObjects->list[i].get())) {
      float detectPercent = guard->detect(gameObjects, playerObject, detectCam);
      detecTrack->updateVisDetect(detectPercent, playerObject);
      //detecTrack->updateVisMeter(dynamic_cast<Player *>(gameObjects->list[i].get()));
      if (detectPercent > 0) {
        detecTrack->detecDanger = true;
        soundObj->guardTalk = soundObj->startSound3D(soundObj->guardTalk, "../dependencies/irrKlang/media/killing_to_me.wav", guard->position);
        detectCounter += detectPercent;
        cout << "Detection: " << detectCounter << " out of " << MAX_DETECT << endl;
        if (detectCounter >= MAX_DETECT) {
          // YOU lose
          cout << "You lose! Not sneaky enough!" << endl;
          soundObj->playSndExit(soundObj->loseSnd);
        }
      }
    }

    // Dirty little visual rep of detection
    if (dynamic_cast<VisualMeter*>(gameObjects->list[i].get())) {
      if (detecTrack->totalDetLvl < 3) {
        gameObjects->list[i].get()->scale.y = detecTrack->totalDetLvl;
      }
      gameObjects->list[i].get()->position = 
        vec3(playerObject->position.x, playerObject->position.y + 1 + detecTrack->totalDetLvl, playerObject->position.z);
      gameObjects->list[i].get()->dimensions.y = detecTrack->totalDetLvl * 2;
      printf("\n totalDetect %f\n", detecTrack->totalDetLvl);
    }
    gameObjects->update();
  }
}

void beginPass1Draw() {
  glBindFramebufferEXT(GL_FRAMEBUFFER, frameBufObj);
  //cerr << "BeginPass1Draw error line 537: " << glGetError() << endl;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //cerr << "BeginPass1Draw error line 539: " << glGetError() << endl;
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_FRONT);
  glDrawBuffer(GL_NONE);
  //cerr << "BeginPass1Draw error: " << glGetError() << endl;
  assert(glGetError() == GL_NO_ERROR);
  //cerr << glGetError() << endl;

  glUseProgram(pass1Handles.prog);
  assert(glGetError() == GL_NO_ERROR);
  //cerr << glGetError() << endl;
  assert(glGetError() == GL_NO_ERROR);
  checkGLError();
}

void endPass1Draw() {
  GLSL::disableVertexAttribArray(pass1Handles.aPosition);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}

void beginPass2Draw() {
  //Second Pass                                                                                                     
  glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
  //glViewport(0, 0, g_width, g_height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);                                                                                        
  glDrawBuffer(GL_BACK);
  glCullFace(GL_BACK);                                                                                            

  glUseProgram(pass2Handles.prog);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, shadowMap);
  glUniform1i(pass2Handles.shadowMap, 0);

  glUniform3f(pass2Handles.uLightPos, g_light.x, g_light.y, g_light.z);
  glUniform3f(pass2Handles.uCamPos, camera3DPerson->eye.x,camera3DPerson->eye.y, camera3DPerson->eye.z); 

  //SetDepthMVP(false);
  if (debug) {
    safe_glUniformMatrix4fv(pass2Handles.uProjMatrix, glm::value_ptr(debugCamera->getProjection()));
    safe_glUniformMatrix4fv(pass2Handles.uViewMatrix, glm::value_ptr(debugCamera->getView()));
  }
  else {
    safe_glUniformMatrix4fv(pass2Handles.uProjMatrix, glm::value_ptr(camera3DPerson->getProjection()));
    safe_glUniformMatrix4fv(pass2Handles.uViewMatrix, glm::value_ptr(camera3DPerson->getView()));
  }
  checkGLError();
}

void endDrawGL() {
  
  GLSL::disableVertexAttribArray(pass2Handles.aPosition);
  GLSL::disableVertexAttribArray(pass2Handles.aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
      playerObject->crouch = false;
      //soundObj->footSndPlayr = soundObj->startSound(soundObj->footSndPlayr, "../dependencies/irrKlang/media/fastWalk.wav");
      //soundObj->footSndPlayr = soundObj->engine->play2D("../dependincies/irrKlang/media/footstepsWalk2.wav", false, true, true);
    }
    else if (action == GLFW_RELEASE) {
      playerObject->SetMotion(WALK);
      playerObject->crouch = false;
      //soundObj->footSndPlayr = soundObj->engine->play2D("../dependincies/irrKlang/media/footstepsWalk2.wav", false, true, true);
    }
  }
  if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
    if (playerObject->crouch) {
      playerObject->SetMotion(WALK);
      playerObject->crouch = false;
    }
    else {
      playerObject->SetMotion(CROUCH);
      playerObject->crouch = true;
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
    if ((phi < max_vert_angle && dy < 0) || (phi > -1.0 * max_vert_angle && dy > 0)) {
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
          vec3(i - (TEST_WORLD/2), 1, j - (TEST_WORLD/2)),
          getRand(0, 360), 
          vec3(4, 2, 4),
          vec3(1.0, 0.0, 0.0),
          0,
          vec3(3.0, 5, 3.0),
          1,
          7,
          false
          )));
          break;
        case 5: //table
          gameObjects->add(shared_ptr<GameObject>(new GameObject(
          &tableMesh,
          vec3(i - (TEST_WORLD/2), -0.50, j - (TEST_WORLD/2)),
          0, 
          vec3(1.5, 1, 1.5),
          vec3(1.0, 0.0, 0.0),
          0,
          vec3(2.8, 1.5, 1.4),
          1,
          5,
          true
          )));
          break;
        case 6: //chair
          gameObjects->add(shared_ptr<GameObject>(new GameObject(
          &chairMesh,
          vec3(i - (TEST_WORLD/2), 0, j - (TEST_WORLD/2)),
          getRand(0, 360), 
          vec3(1, 1, 1),
          vec3(1.0, 0.0, 0.0),
          0,
          vec3(1.5, 2, 1.5),
          1,
          8,
          true
          )));
          break;
        case 7: //cart
          gameObjects->add(shared_ptr<GameObject>(new GameObject(
          &cartMesh,
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
          gameObjects->add(shared_ptr<GameObject>(new WinCondition(
          &winMesh,
          vec3(i - (TEST_WORLD/2), 3.2, j - (TEST_WORLD/2)),
          0, 
          vec3(4, 6, 4),
          vec3(1.0, 0.0, 0.0),
          0,
          vec3(1, 10, 1),
          1,
          3
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
      vec3(0, 0, 0),
      20,
      vec3(1.0, 1.0, 1.0), //scale
      vec3(1, 0, 0),
      vec3(0.75, 2.0, 0.75), // dimensions
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
            vec3(center.x, 0, center.y),      //position
            0,            //rotation
            vec3(dims.x / 2, 1, dims.y / 2),    //scale
            vec3(1, 0, 0),  //direction
            0,
            vec3(dims.x, 2, dims.y),     //dimensions
            0,            //scanRadius
            6             //material
            )));
            shortWall = false;     
        }
        else {
				gameObjects->add(shared_ptr<GameObject>(new Wall(
					&cubeMesh,
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

void initDetectionTracker(WorldGrid* gameObjects) {
  gameObjects->add(shared_ptr<GameObject>(detecTrack->visMeter = new VisualMeter(
    &cubeMesh,
    vec3(playerObject->position.x, playerObject->dimensions.y + 1, playerObject->position.z),      //position
    0,                //rotation
    vec3(0.2, 0, 0.2),    //scale
    vec3(1, 0, 0),    //direction
    0,
    vec3(0, 0, 0),     //dimensions
    0,                 //scanRadius
    0                  //material
    )));
}

int main(int argc, char **argv)
{
  // Sound Object
  soundObj = new MySound();

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
  assert(glGetError() == GL_NO_ERROR);

  debugDraw = new DebugDraw();
  debugDraw->installShaders(resPath(sysPath("shaders", "vert_debug.glsl")), resPath(sysPath("shaders", "frag_debug.glsl")));
  pass1Handles.installShaders(resPath(sysPath("shaders", "pass1Vert.glsl")), resPath(sysPath("shaders", "pass1Frag.glsl")));
  pass2Handles.installShaders(resPath(sysPath("shaders", "pass2Vert.glsl")), resPath(sysPath("shaders", "pass2Frag.glsl")));
  assert(glGetError() == GL_NO_ERROR);

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
  detecTrack = new DetectionTracker();

  initPlayer(&gameObjects);
  initGuards(&gameObjects);
  initObjects(&gameObjects);
  initWalls(&gameObjects);
  initGround();
  initCeiling();
  initDetectionTracker(&gameObjects);
      
    //initialize the camera
  camera3DPerson = new Camera3DPerson(&gameObjects, playerObject, CAMERA_ZOOM, CAMERA_FOV,
      (float)g_width / (float)g_height,
      CAMERA_NEAR, CAMERA_FAR, debugDraw);
  // debug camera
  debugCamera = new Camera(glm::vec3(0.0f, 0.0f, 1.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      CAMERA_FOV,
      (float)g_width / (float)g_height,
      CAMERA_NEAR,
      CAMERA_FAR,
      debugDraw);
  detectCam = new DetectionCamera(CAMERA_FOV,
      (float)g_width / (float)g_height,
      CAMERA_NEAR,
      CAMERA_FAR,
      debugDraw);

  double timeCounter = 0;

  do{
    //timer stuff
    TimeManager::Instance().CalculateFrameRate(true);
    deltaTime = TimeManager::Instance().DeltaTime;
    double currentTime = TimeManager::Instance().CurrentTime;
    timeCounter += deltaTime;

    camera3DPerson->update();
    beginPass1Draw();
    drawPass1(&gameObjects);
    endPass1Draw();
    beginPass2Draw();
    getWindowinput(window, deltaTime);
    drawGameObjects(&gameObjects, deltaTime);
    endDrawGL();

    // draw debug
    if (debug || boxes) {
      if (debug) {
        camera3DPerson->getView();
        camera3DPerson->getProjection();
        debugDraw->view = debugCamera->getView();
        debugDraw->projection = debugCamera->getProjection();
      }
      else {
        debugDraw->view = camera3DPerson->getView();
        debugDraw->projection = camera3DPerson->getProjection();
      }

      vector<shared_ptr<GameObject>> objs = gameObjects.list;
      for (auto objIter = objs.begin(); objIter != objs.end(); ++objIter) {
        debugDraw->addBox((*objIter)->position, (*objIter)->dimensions, glm::vec3(0.7f, 0.1f, 1.0f), false, true);
      }

      vector<shared_ptr<GameObject>> walls = gameObjects.wallList;
      for (auto objIter = walls.begin(); objIter != walls.end(); ++objIter) {
        debugDraw->addBox((*objIter)->position, (*objIter)->dimensions, glm::vec3(0.7f, 0.1f, 1.0f), false, true);
      }
    }
    if (debug || boxes) {
#ifndef WIN32
      debugDraw->drawAll();
#endif
      debugDraw->clear();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
      && glfwWindowShouldClose(window) == 0);

  glfwTerminate();
  soundObj->cleanUpSound();
  return 0;
}
