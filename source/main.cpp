#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
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
#include "GameObject/ShadowMapPass1Handles.h"
#include "GameObject/ShadowMapPass2Handles.h"
#include "GameObject/Mesh.h"
#include "GameObject/WinCondition.h"
#include "GameObject/DetailProp.h"
#include "GameObject/VisualMeter.h"
#include "Camera/Camera.h"
#include "Camera/Camera3DPerson.h"
#include "Camera/DetectionCamera.h"
#include "Path/Path.h"
#include "Deferred/gbuffer.h"
#include "Deferred/dbuffer.h"
#include "Deferred/LightPassHandles.h"
#include "Deferred/Light.h"
#include "Deferred/GeometryPassHandles.h"
#include "WorldGrid/WorldGrid.h"
#include "MySound/MySound.h"
#include "Textures/Textures.h"
#include "DetectionTracker/DetectionTracker.h"

#include "HUD/imgui.h"
#include "HUD/imgui_impl_glfw.h"

#include "GameObject/Clue.h"

#define DEBUG

#define WORLD_WIDTH 300
#define WORLD_HEIGHT 300
#define TEST_WORLD 200

#define CAMERA_FOV 60.0f
#define CAMERA_NEAR 0.1f
#define CAMERA_FAR 200.0f
#define CAMERA_ZOOM 3.0f
#define CAMERA_SPEED 10.0f
#define GUARD_FAR 20.0f
#define GUARD_FOV 70.0f

#define GUARD_SPEED 4.0f
#define BOTTOM_LEVEL 1.0f
#define MID_LEVEL 2.0f
#define TOP_LEVEL 3.0f

#define TEX_SIZE 1024

GLFWwindow* window;
using namespace std;
using namespace glm;

vector<Light> gLights;

vector<tinyobj::shape_t> player;
vector<tinyobj::shape_t> guard;
vector<tinyobj::material_t> materials;
vector<tinyobj::shape_t> wall;

int g_width;
int g_height;
int g_framebuffer_width;
int g_framebuffer_height;

float key_speed = 0.2f; // TODO get rid of these by implementing first-person camera
float theta = 0.0f;
float phi = 0.0f;
Camera *debugCam;
Camera *cineCam;
Camera *drawCam;  // camera that's used for drawing (vfc, shading, etc.)
Camera *viewCam;  // camera that's used for setting the view (controlled by mouse input)
Camera3DPerson *camera3DPerson;
Player* playerObject;
vec3 oldPosition;
Pass1Handles depthHandles;
Pass2Handles pass2Handles;
GeometryPassHandles geomHandles;
LightPassHandles lightHandles;
GBuffer m_gbuffer;
DBuffer m_dbuffer;
Mesh guardMesh;
Mesh playerMesh;
Mesh cubeMesh;
Mesh shortCubeMesh;
Mesh barrel;
Mesh boxStackMesh;
Mesh tableMesh;
Mesh chairMesh;
Mesh lightMesh;
Mesh rafterMesh;
Mesh winMesh;
Mesh trainMesh;
Mesh clueMesh;
Mesh printMesh;
Mesh coneMesh;
Shape *ground;
Shape *ceiling;
bool debug = false;
bool boxes = false;
bool shiftDown = false;
bool inIntro = true;
double introDist = 0;
// introduction time, in seconds
float introTime = 80;
DebugDraw *debugDraw;
DetectionCamera *detectCam;
MySound *soundObj;
DetectionTracker *detecTrac;
#ifdef DEBUG
ofstream curveOutput;
#endif
SplineCurve introCurve; // add points
float guardDetecDir;

//std::vector<glm::vec3> lights;
glm::vec3 g_light(0.0, 15.0, -2.0);
// increasing this makes distance attenuation sharper (MAKE SURE IT ISN'T EVER ALL ZERO!!!!!1)
//glm::vec3 lightAtten(0.0, 0.0001, 0.0002);
glm::vec3 coneDir(0.0, -0.5, 0.0);
float coneAngle = 20;
int closestLNdx = 0;
GLuint posBufObjG = 0;
GLuint norBufObjG = 0;
GLuint idxBufObjG = 0;
//GLuint frameBufObj[MAX_LIGHTS] = {0};
GLuint texBufObjG = 0;
//GLuint shadowMap[MAX_LIGHTS] = {0};

double deltaTime = 0;
double timeCounter = 0;
double currentTime = 0;
double camSpeed = 450;

GLuint texture;

float G_edge = 30;
GLfloat planeVertices[] = {
  -G_edge, -1.0f, -G_edge,
  -G_edge, -1.0f, G_edge,
  G_edge, -1.0f, -G_edge,
  -G_edge, -1.0f, G_edge,
  G_edge, -1.0f, -G_edge,
  G_edge, -1.0f, G_edge,
};

GLfloat planeNormals[] = {
  0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f,
};


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

/*void SetMaterial(int i) {
  switch (i) {
  case 0: // guards
    glUniform3f(pass2Handles.uMatAmb, 0.05f, 0.025f, 0.025f);
    glUniform3f(pass2Handles.uMatDif, 0.9f, 0.1f, 0.05f);
    glUniform3f(pass2Handles.uMatSpec, 0.8f, 0.2f, 0.2f);
    glUniform1f(pass2Handles.uMatShine, 100.0f);
    break;
  case 1: // floor
    glUniform3f(pass2Handles.uMatAmb, 0.13f, 0.13f, 0.14f);
    glUniform3f(pass2Handles.uMatDif, 0.3f, 0.3f, 0.4f);
    glUniform3f(pass2Handles.uMatSpec, 0.3f, 0.3f, 0.4f);
    glUniform1f(pass2Handles.uMatShine, 150.0f);
    break;
  case 2: // player
    glUniform3f(pass2Handles.uMatAmb, 0.3f, 0.3f, 0.3f);
    glUniform3f(pass2Handles.uMatDif, 0.9f, 0.9f, 0.9f);
    glUniform3f(pass2Handles.uMatSpec, 0.0f, 0.0f, 0.0f);
    glUniform1f(pass2Handles.uMatShine, 150.0f);
    break;
  case 3: // guard detect
    glUniform3f(pass2Handles.uMatAmb, 0.06f, 0.09f, 0.06f);
    glUniform3f(pass2Handles.uMatDif, 0.2f, 0.80f, 0.1f);
    glUniform3f(pass2Handles.uMatSpec, 0.8f, 1.0f, 0.8f);
    glUniform1f(pass2Handles.uMatShine, 4.0f);
    break;
  case 4: //big wall color
    glUniform3f(pass2Handles.uMatAmb, 0.2f, 0.1f, 0.0f);
    glUniform3f(pass2Handles.uMatDif, 0.08f, 0.0f, 0.00f);
    glUniform3f(pass2Handles.uMatSpec, 0.08f, 0.0f, 0.0f);
    glUniform1f(pass2Handles.uMatShine, 10.0f);
    break;
  case 5: // ceiling
    glUniform3f(pass2Handles.uMatAmb, 0.1f, 0.1f, 0.1f);
    glUniform3f(pass2Handles.uMatDif, 0.0f, 0.0f, 0.00f);
    glUniform3f(pass2Handles.uMatSpec, 1.0f, 1.0f, 1.0f);
    glUniform1f(pass2Handles.uMatShine, 100.0f);
    break;
  case 6: //short wall color
    glUniform3f(pass2Handles.uMatAmb, 0.2f, 0.2f, 0.2f);
    glUniform3f(pass2Handles.uMatDif, 0.08f, 0.0f, 0.00f);
    glUniform3f(pass2Handles.uMatSpec, 0.08f, 0.0f, 0.0f);
    glUniform1f(pass2Handles.uMatShine, 10.0f);
    break;
  case 7: //box stack
    glUniform3f(pass2Handles.uMatAmb, 0.39f, 0.20f, 0.1f);
    glUniform3f(pass2Handles.uMatDif, 0.28f, 0.1f, 0.00f);
    glUniform3f(pass2Handles.uMatSpec, 0.08f, 0.0f, 0.0f);
    glUniform1f(pass2Handles.uMatShine, 10.0f);
    break;
  case 8: //chairs
    glUniform3f(pass2Handles.uMatAmb, 0.05f, 0.1f, 0.25f);
    glUniform3f(pass2Handles.uMatDif, 0.1f, 0.15f, 0.2f);
    glUniform3f(pass2Handles.uMatSpec, 0.08f, 0.0f, 0.1f);
    glUniform1f(pass2Handles.uMatShine, 10.0f);
    break;
  case 9: //footprint
    glUniform3f(pass2Handles.uMatAmb, 1.0f, 1.0f, 1.0f);
    glUniform3f(pass2Handles.uMatDif, 1.0f, 1.0f, 1.0f);
    glUniform3f(pass2Handles.uMatSpec, 0.08f, 0.0f, 0.1f);
    glUniform1f(pass2Handles.uMatShine, 1.0f);
  }
}*/

void readIntroSpline()
{
  ifstream curveInput;
  curveInput.open(resPath("introCurve.txt"));
  double x, y, z;
  while (curveInput >> x) {
    curveInput >> y;
    curveInput >> z;
    introCurve.addPoint(x, y, z);
  }
  curveInput.close();
}

void SetDepthMVP(bool pass1, glm::mat4 depthModelMatrix, Light g_light) {
  //WE WANT SPOT LIGHTS USE PERSPECTIVE MATRIX INSTEAD!!!!
  /*glm::vec3 lightInv = glm::vec3(0.5f, 2, 2);*/
  /*const float invTanHalfFov = 1.0f / std::tan(g_light.coneAngle * 0.5f);
  const float nearClipPlane = 0.1f;
  const float farClipPlane = 20.0f;
  const float zRange = nearClipPlane - farClipPlane;
  const glm::mat4 depthProjMatrix(
      invTanHalfFov, 0.0f, 0.0f, 0.0f,
      0.0f, invTanHalfFov, 0.0f, 0.0f,
      0.0f, 0.0f, -(nearClipPlane + farClipPlane) / zRange, 2.0f * nearClipPlane * farClipPlane / zRange,
      0.0f, 0.0f, 1.0f, 0.0f
      );*/
  glm::mat4 depthProjMatrix = glm::perspective<float>((1.0f / (std::tan(g_light.angle))), 1.0f, 0.1f, 20.0f);
  glm::mat4 depthViewMatrix = glm::lookAt(g_light.position, g_light.position - g_light.direction, glm::vec3(1, 0, 0));
  glm::mat4 depthMVP = depthProjMatrix * depthViewMatrix * depthModelMatrix;

  glm::mat4 biasMatrix(
      0.5, 0.0, 0.0, 0.0,
      0.0, 0.5, 0.0, 0.0,
      0.0, 0.0, 0.5, 0.0,
      0.5, 0.5, 0.5, 1.0
      );

  glm::mat4 depthBiasMVP = biasMatrix*depthMVP;

   pass1 ? safe_glUniformMatrix4fv(depthHandles.uDepthMVP, glm::value_ptr(depthMVP)) :
     safe_glUniformMatrix4fv(lightHandles.uDepthMVP, glm::value_ptr(depthBiasMVP));

  //cerr << glGetError() << endl;
  //  assert(glGetError() == GL_NO_ERROR);
  checkGLError();
}

void initGL() {
  // Set the background color
  glClearColor(0.0f, 0.0f, 0.9f, 1.0f);
  // Enable Z-buffer test
  glEnable(GL_DEPTH_TEST);
  glPointSize(18);
  initVertexObject(&posBufObjG, &norBufObjG, &idxBufObjG, &texBufObjG, planeVertices, planeNormals, G_edge);
  //initLights();
}

void getWindowInput(GLFWwindow* window, double deltaTime) {
  float forwardYVelocity = 0;
  float sideYVelocity = 0;
  bool accelerate = false;
  bool upD = false;
  bool downD = false;
  bool leftD = false;
  bool rightD = false;
  static bool leaningLeft = false;
  static bool leaningRight = false;
  static vec3 cameraLean;
  vec3 direction(0, 0, 0);
  glm::vec3 forward = camera3DPerson->getForward();
  glm::vec3 strafe = camera3DPerson->getStrafe();
  glm::vec3 up = camera3DPerson->getUp();
  oldPosition = playerObject->position;

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    shiftDown = true;
  }
  else {
    shiftDown = false;
  }
  if (!inIntro) {
    if (!debug) {
      if (!leaningRight && ! leaningLeft) {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
          vec3 velocity = glm::vec3(strafe.x * CAMERA_SPEED * deltaTime,
              sideYVelocity, strafe.z * CAMERA_SPEED * deltaTime);
          velocity.y = 0;
          direction += -velocity;
          glm::vec3 forward = camera3DPerson->getForward();
          //playerObject->rotation = atan2f(-velocity.x, -velocity.z) * 180 / M_PI;
          accelerate = true;
          leftD = true;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
          vec3 velocity = glm::vec3(strafe.x * CAMERA_SPEED * deltaTime,
              sideYVelocity, strafe.z * CAMERA_SPEED * deltaTime);
          velocity.y = 0;
          direction += velocity;
          glm::vec3 forward = camera3DPerson->getForward();
          //playerObject->rotation = atan2f(velocity.x, velocity.z) * 180 / M_PI;
          accelerate = true;
          rightD = true;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
          vec3 velocity = glm::vec3(forward.x * CAMERA_SPEED * deltaTime,
              forwardYVelocity, forward.z * CAMERA_SPEED * deltaTime);
          velocity.y = 0;
          direction += velocity;
          glm::vec3 forward = camera3DPerson->getForward();
          //playerObject->rotation = atan2f(velocity.x, velocity.z) * 180 / M_PI;
          accelerate = true;
          upD = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
          vec3 velocity = glm::vec3(forward.x * CAMERA_SPEED * deltaTime,
              forwardYVelocity, forward.z * CAMERA_SPEED * deltaTime);
          velocity.y = 0;
          direction += -velocity;
          glm::vec3 forward = camera3DPerson->getForward();
          //playerObject->rotation = atan2f(-velocity.x, -velocity.z) * 180 / M_PI;
          accelerate = true;
          downD = true;
        }
      }

      float shearSpeed = 3;
      float camShiftSpeed = 6;
      float iters = 20;
      float max_lean = 0.5;
      if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (!leaningRight) {
          if (!leaningLeft) {
            playerObject->handleLeanSound(true);
            leaningLeft = true;
            cameraLean = strafe;
          }
          if (playerObject->lean < max_lean) {
            glm::vec3 newForward = forward;
            newForward.y = 0;
            playerObject->changeDirection(newForward);
          }
          for (int i = 0; i < iters && playerObject->lean < max_lean; ++i) {
            playerObject->justLeaned = true;
            playerObject->lean += deltaTime * shearSpeed / iters;
            camera3DPerson->offset -= ((float)deltaTime * camShiftSpeed / iters) * cameraLean;
          }
        }
      }
      else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
        if (leaningLeft) {
          for (int i = 0; i < iters && playerObject->lean > 0; ++i) {
            playerObject->lean -= deltaTime * shearSpeed / iters;
            camera3DPerson->offset += ((float)deltaTime * camShiftSpeed / iters) * cameraLean;
          }
          if (playerObject->lean <= 0) {
            leaningLeft = false;
            playerObject->handleLeanSound(false);
          }
        }
      }
      if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!leaningLeft) {
          if (!leaningRight) {
            playerObject->handleLeanSound(true);
            leaningRight = true;
            cameraLean = strafe;
          }
          if (playerObject->lean > -1 * max_lean) {
            glm::vec3 newForward = forward;
            newForward.y = 0;
            playerObject->changeDirection(newForward);
          }
          for (int i = 0; i < iters && playerObject->lean > -1 * max_lean; ++i) {
            playerObject->lean -= deltaTime * shearSpeed / iters;
            camera3DPerson->offset += ((float)deltaTime * camShiftSpeed / iters) * cameraLean;
          }
        }
      }
      else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
        if (leaningRight) {
          for (int i = 0; i < iters && playerObject->lean < 0; ++i) {
            playerObject->lean += deltaTime * shearSpeed / iters;
            camera3DPerson->offset -= ((float)deltaTime * camShiftSpeed / iters) * cameraLean;
          }
          if (playerObject->lean >= 0) {
            leaningRight = false;
            playerObject->handleLeanSound(false);
          }
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
      playerObject->decelerate(); // fixes bug where player keeps moving in debug mode
      glm::vec3 view = -1.0f * debugCam->getForward();
      glm::vec3 up = debugCam->getUp();
      glm::vec3 strafe = debugCam->getStrafe();
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
      debugCam->eye += move;
      debugCam->lookat += move;
    }
  }

  // change mouse sensitivity
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    camSpeed *= 1.01;
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    camSpeed *= 0.99;
  }
}

int findClosestLight() {
  int ndx = 0;
  float closest = glm::distance(playerObject->position, gLights.at(0).position);

  for (int i = 0; i < gLights.size(); i++) {
    float dist = glm::distance(playerObject->position, gLights.at(i).position);
    if (dist < closest) {
      ndx = i;
      closest = dist;
    }
  }

  return ndx;

}

/*void beginPass1Draw() {
  closestLNdx = findClosestLight();
  glBindFramebufferEXT(GL_FRAMEBUFFER, frameBufObj[closestLNdx]);
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
}*/

void SetLightUniform(Light light, int ndx) {
  if (ndx < MAX_LIGHTS) {
  ostringstream stream;
  ///Array of handles
    stream << "uLightPos[" << ndx << "]";
  checkGLError();
  //printf("light %d position %lf %lf %lf\n", ndx, light.position.x, light.position.y, light.position.z);
    //lightHandles.uLightPos[ndx] = GLSL::getUniformLocation(lightHandles.prog, stream.str().c_str());
    lightHandles.uLightPos[ndx] = GLSL::getUniformLocation(lightHandles.prog, "uLightPos[0]");
  //printf("handle allLights: %d\n", pass2Handles.uAllLights[ndx]);
  checkGLError();
    glUniform3f(lightHandles.uLightPos[ndx], light.position.x, light.position.y, light.position.z);
  }
}

/*void beginPass2Draw() {
  //Second Pass
  glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDrawBuffer(GL_BACK);
  glCullFace(GL_BACK);

  glUseProgram(pass2Handles.prog);
  glEnable(GL_TEXTURE_2D);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, shadowMap[closestLNdx]);
  glUniform1i(pass2Handles.shadowMap, 0);
  glActiveTexture(GL_TEXTURE1);

  glUniform1i(pass2Handles.uNumLights, (int)gLights.size());
  for (int i = 0; i < gLights.size(); i++) {
    SetLightUniform(gLights[i], i);
  }

  checkGLError();
  //glUniform3f(pass2Handles.uLightPos, gLights.at(0).position.x, gLights.at(0).position.y, gLights.at(0).position.z);
  glUniform3f(pass2Handles.uConeDirection, coneDir.x, coneDir.y, coneDir.z);
  glUniform1f(pass2Handles.uConeAngle, coneAngle);
  glUniform3f(pass2Handles.uCamPos, drawCam->eye.x,drawCam->eye.y, drawCam->eye.z);
  glUniform1i(pass2Handles.hasTex, 0);

  safe_glUniformMatrix4fv(pass2Handles.uProjMatrix, glm::value_ptr(viewCam->getProjection()));
  safe_glUniformMatrix4fv(pass2Handles.uViewMatrix, glm::value_ptr(viewCam->getView()));

  checkGLError();
}*/

//void inLightCalc(vec3 first, vec3 second, float max, Light thisLight) {
//  float deltaX = first.x - second.x;
//  float deltaZ = first.z - second.z;
//
//  float dist = deltaX * deltaX +  deltaZ * deltaZ;
//  bool returnVal;
//
//  // NOTE: the + 80.0f there is to make sure the light is saved if the player get NEAR the light
//  // this does not mean it will count against the player, the detecTrac will handle that
//  if (dist < ((max * max) + 80.0f)) {
//    detecTrac->currLight = thisLight;
//    printf("Updating current light comparing dist: %f and maxSq: %f\n", dist, (max*max));
//  }
//  else {
//    return;
//  }
//}

Light getClosestLight(vector<Light> gLights, Player *player) {
  Light lightWithMinDist;
  float min = ((gLights[0].position.x - player->position.x) * (gLights[0].position.x - player->position.x)) + ((gLights[0].position.z - player->position.z) * (gLights[0].position.z - player->position.z));
   for (int L = 1; L < gLights.size(); L++) {
	   if (!gLights[L].guardLight) {
		   float deltaX = gLights[L].position.x - player->position.x;
		   float deltaZ = gLights[L].position.z - player->position.z;

		   float dist = deltaX * deltaX + deltaZ * deltaZ;
		   //printf("comparing dist: %f with min:  %f\n", dist, min);
		   if (dist < min) {
			   min = dist;
			   //printf("Choose %f, light indx: %d\n", min, L);
			   lightWithMinDist = gLights[L];
		   }
	   }
   }

   return lightWithMinDist;
}

#define FORWARD_LIMIT 0.8f
#define BACK_LIMIT -0.8f
// 1 = forward, 2 = backward, 3 = left, 4 = right
float calculateGuardDetecDir(Player *player, Guard *guard, Camera3DPerson *camera) {
  vec3 cameraDir = camera->getForward();
  cameraDir.y = 0;
  cameraDir = normalize(cameraDir);
  //float xDiff = (guard->position.x - cameraDir.x) * (guard->position.x - cameraDir.x);
  //float zDiff = (guard->position.z - cameraDir.z) * (guard->position.z - cameraDir.z);
  float retVal, sideOfView;
  //printf("xDiff: %f, zDiff: %f\n", xDiff, zDiff);
  vec3 ptog = guard->position - player->position;
  ptog.y = 0.0f;
  ptog = normalize(ptog);
  vec3 crossProd = cross(cameraDir, ptog);
  float crossMag = crossProd.y;
  float dotProd = dot(cameraDir, ptog);

  //printf("crossProd: %f, %f, %f\n", crossProd.x, crossProd.y, crossProd.z);
  //printf("cross %f : dot %f \n", crossMag, dotProd);
  if (dotProd < FORWARD_LIMIT && dotProd > BACK_LIMIT) { // to a side
    //float sideOfView = cameraDir.x * guard->position.x + cameraDir.y * guard->position.y + cameraDir.z * guard->position.z;
    //printf("side of plane: %f\n", sideOfView);
    if (crossMag < 0) {
      retVal = 4.0;
    }
    else {
      retVal = 3.0;
    }
  }
  else {
    //float dotProd = dot(cameraDir, guard->position);
    if (dotProd > 0) {
        retVal = 1.0;
    }
    else {
        retVal = 2.0;
    }
  }
  //printf("returnVal: %f\n", retVal);
  return retVal;
}

void passDetectDirection(float guardDetectDir) {
  vec2 pt1, pt2, pt3;
  // Forward Triangle
  if (guardDetectDir == 1.0) {
    pt1 = vec2(565, 570);
    pt2 = vec2(715, 570);
    pt3 = vec2(640, 645);
  }
  // Backward Triangle
  else if (guardDetectDir == 2.0) {
    pt1 = vec2(565, 125);
    pt2 = vec2(715, 125); 
    pt3 = vec2(640, 50);
  }
  // Left Triangle
  else if (guardDetectDir == 3.0) {
    pt1 = vec2(100, 360);
    pt2 = vec2(175, 435);
    pt3 = vec2(175, 285);
  }
  // Right Triangle
  else if (guardDetectDir == 4.0) {
    pt1 = vec2(1180, 360);
    pt2 = vec2(1105, 435);
    pt3 = vec2(1105, 285);
  }

  //printf("derectDir: %f\n", guardDetectDir);

  glUniform2f(lightHandles.pt1, pt1.x, pt1.y);
  glUniform2f(lightHandles.pt2, pt2.x, pt2.y);
  glUniform2f(lightHandles.pt3, pt3.x, pt3.y);
  glUniform1i(lightHandles.detecDir, guardDetectDir);

  // #ifdef __APPLE__
  // glUniform2f(lightHandles.pt1, pt1.x*2, pt1.y*2);
  // glUniform2f(lightHandles.pt2, pt2.x*2, pt2.y*2);
  // glUniform2f(lightHandles.pt3, pt3.x * 2, pt3.y * 2);
  // #endif

}

//PASS different number of lights! Not a different number of renderings?
void drawGameObjects(WorldGrid* gameObjects, float time) {
  Guard *guard;
  Clue *clue;
  guardDetecDir = 0.0;
  //    for (int l = 0; l < gLights.size(); l++) {
  //glUniform1i(geomHandles.hasTex, 1);
  closestLNdx = findClosestLight();
  glBindTexture(GL_TEXTURE_2D, ground->texId);
  glUniform1i(geomHandles.texture, 0);
  //SetMaterial(0);
  //SetDepthMVP(false, ground->getModel(), gLights.at(closestLNdx));
  safe_glUniformMatrix4fv(geomHandles.uModel, glm::value_ptr(ground->getModel()));
  geomHandles.draw(ground);

  //  glUniform1i(pass2Handles.hasTex, 1);
  glBindTexture(GL_TEXTURE_2D, ceiling->texId);
  glUniform1i(geomHandles.texture, 0);
  //SetMaterial(0);
  //SetDepthMVP(false, ceiling->getModel(), gLights.at(closestLNdx));
  safe_glUniformMatrix4fv(geomHandles.uModel, glm::value_ptr(ceiling->getModel()));
  geomHandles.draw(ceiling);

  //ceiling->draw();
  //Guard *guard;
  // draw
  vector<shared_ptr<GameObject>> drawList = drawCam->getUnculled(gameObjects);
  for (int i = 0; i < drawList.size(); i++) {
    if (drawList[i]->mesh->hasTexture) {
      //glUniform1i(pass2Handles.hasTex, 1);
      //printf("bound texture for game object\n");
      glBindTexture(GL_TEXTURE_2D, drawList[i]->mesh->texId);
      glUniform1i(geomHandles.texture, 0);
      //SetMaterial(0);
    }
    else {
      //glUniform1i(pass2Handles.hasTex, 0);
      //SetMaterial(drawList[i]->material);
    }
    
    /*if(drawList[i]->type == GameObject::ObjectType::COLLECTABLE) {
      //glUniform1i(pass2Handles.isGlass, 1);
    }
    else {
      //glUniform1i(pass2Handles.isGlass, 0);
    }*/
    
    // SetMaterial(drawList[i]->material);
    //SetDepthMVP(false, drawList[i]->getModel(), gLights.at(closestLNdx));
    safe_glUniformMatrix4fv(geomHandles.uModel, glm::value_ptr(drawList[i]->getModel()));
    geomHandles.draw(drawList[i].get());
    //drawList[i]->draw();
  }
  //}

  // collide and move
  for (int i = 0; i < gameObjects->list.size(); i++) {
    gameObjects->list[i]->move(time);
    vector<shared_ptr<GameObject>> proximity =
      gameObjects->getCloseObjects(gameObjects->list[i]);

    //all objects
    for (int j = 0; j < proximity.size(); j++) {
      if (gameObjects->list[i].get() != proximity[j].get()) {
        if (gameObjects->list[i]->collide(proximity[j].get(), debugDraw)) {
          //do some stuff
        }
      }
    }


    //guards
    if (guard = dynamic_cast<Guard*>(gameObjects->list[i].get())) {
      float detectPercent = guard->detect(gameObjects, detectCam, detecTrac);
      if (detectPercent > 0) {
		    if (detecTrac->totalDetLvl > 0.5 || guard->staring) {
			    guard->stare();
		    }
        detecTrac->detecDanger = true;
        detecTrac->updateVisDetect(detectPercent, playerObject);
        guardDetecDir = calculateGuardDetecDir(playerObject, guard, camera3DPerson);
      }
    }
    
    if (dynamic_cast<Player *>(gameObjects->list[i].get())) {
		detecTrac->updateSndDetect(playerObject);
      detecTrac->currLight = getClosestLight(gLights, dynamic_cast<Player *>(gameObjects->list[i].get()));
        detecTrac->detecDanger = false;
      }

    if (clue = dynamic_cast<Clue *>(gameObjects->list[i].get())) {
      if (clue->isCollected) {
        gameObjects->remove(i);
      }
    }

    checkGLError();
    //glUniform1f(pass2Handles.detectionLevel, detecTrac->totalDetLvl);
    //glUniform1f(lightHandles.uDetectionLevel, detecTrac->totalDetLvl);
    checkGLError();
    gameObjects->update();
  }
}
void drawShadowPass(WorldGrid* gameObjects) {
  Guard *guard;
  // draw
  //vector<shared_ptr<GameObject>> drawList = camera3DPerson->getUnculled(gameObjects);
  //pass1Handles.draw(ground);
  //closestLNdx = findClosestLight();
  //printf("closestLNdx %d\n", closestLNdx);
  vector<shared_ptr<GameObject>> drawList = gameObjects->list;
  vector<shared_ptr<GameObject>> walls = gameObjects->wallList;
  drawList.insert(drawList.end(), walls.begin(), walls.end());
  //  for (int l = 0; l < gLights.size(); l++) {
  for (int i = 0; i < drawList.size(); i++) {
    SetDepthMVP(true, drawList[i]->getModel(), gLights.at(closestLNdx));
    depthHandles.draw(drawList[i].get());
    //drawList[i]->draw();
}
  //}

  gameObjects->update();
}

void shadowPass(WorldGrid* gameObjects) {
 
  m_dbuffer.start();

  glUseProgram(depthHandles.prog); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_FRONT); 

  drawShadowPass(gameObjects);

  m_dbuffer.stop();
  glCullFace(GL_BACK); 
  glDisable(GL_CULL_FACE);
  assert(glGetError() == GL_NO_ERROR);
}

void geometryPass(WorldGrid* gameObjects, float time) {
  m_gbuffer.start();
  checkGLError();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  checkGLError();

  glUseProgram(geomHandles.prog);
  checkGLError();

  safe_glUniformMatrix4fv(geomHandles.uProj, glm::value_ptr(viewCam->getProjection()));
  safe_glUniformMatrix4fv(geomHandles.uView, glm::value_ptr(viewCam->getView()));
 
  glActiveTextureARB(GL_TEXTURE1_ARB);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.getPosTexture());

  glActiveTextureARB(GL_TEXTURE2_ARB);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.getNormTexture());

  glActiveTextureARB(GL_TEXTURE0_ARB);
  glEnable(GL_TEXTURE_2D);

  drawGameObjects(gameObjects, time);

  /*  GLSL::disableVertexAttribArray(geomHandles.aPosition);
  GLSL::disableVertexAttribArray(geomHandles.aNormal);
  GLSL::disableVertexAttribArray(geomHandles.aTexCoord);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);*/

  //m_gbuffer.stop();
  //glUseProgram(0);
  //checkGLError();
}

/*void beginLightPass() {
  // glEnable(GL_BLEND);
  checkGLError();
  //glBlendEquation(GL_FUNC_ADD);
  checkGLError();
  //glBlendFunc(GL_ONE, GL_ONE);
  checkGLError();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}*/

void lightPass() {
  glUseProgram(lightHandles.prog);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  m_gbuffer.stop();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  checkGLError();

  glActiveTextureARB(GL_TEXTURE0_ARB);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.getDiffTexture());
  glUniform1i(lightHandles.uColMap, 0);

  glActiveTextureARB(GL_TEXTURE1_ARB);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.getPosTexture());
  glUniform1i(lightHandles.uPosMap, 1);

  glActiveTextureARB(GL_TEXTURE2_ARB);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.getNormTexture());
  glUniform1i(lightHandles.uNormMap, 2);
  glUniform1f(lightHandles.uAmbient, 0.1);

  glActiveTexture(GL_TEXTURE3);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, m_dbuffer.getDepthTex());
  glUniform1i(lightHandles.uDepthMap, 3);

  glUniform2f(lightHandles.uScreenSize, g_width, g_height);
  glUniform1f(lightHandles.uDetectionLevel, detecTrac->totalDetLvl);
  
  glUniform3f(lightHandles.uLightCol, 1.0, 1.0, 1.0);
  //glUniform3fv(lightHandles.uLightCol, 1, glm::value_ptr(gLights.at(0).color));
  //glUniform3fv(lightHandles.uLightAtten, 1, glm::value_ptr(gLights.at(0).atten));
  glUniform3f(lightHandles.uLightAtten, 0.0, 0.0001, 0.0002);
  //glUniform3fv(lightHandles.uLightDirection, 1, glm::value_ptr(gLights.at(0).direction));
  //glUniform3f(lightHandles.uLightDirection, 0.0, -1.0, 0.0);
  glUniform1f(lightHandles.uLightAngleCos, cos(15.0f * M_PI / 180));

  glUniform1i(lightHandles.uNumLights, (int)gLights.size());
  passDetectDirection(guardDetecDir);

  GLfloat positions[MAX_LIGHTS * 3];
  GLfloat directions[MAX_LIGHTS * 3];
  for (int i = 0; i < gLights.size(); i++) {
    //SetLightUniform(gLights.at(i), i);
    positions[3*i+0] = gLights.at(i).position.x;
    positions[3*i+1] = gLights.at(i).position.y;
    positions[3*i+2] = gLights.at(i).position.z;
    directions[3*i+0] = gLights.at(i).direction.x;
    directions[3*i+1] = gLights.at(i).direction.y;
    directions[3*i+2] = gLights.at(i).direction.z;
    SetDepthMVP(false, glm::mat4(1.0f), gLights.at(i));
  }
  glUniform3fv(glGetUniformLocation(lightHandles.prog, "uLightPos"), gLights.size(), positions);
  glUniform3fv(glGetUniformLocation(lightHandles.prog, "uLightDirection"), gLights.size(), directions);
  lightHandles.drawQuad();

  //for (int i = 0; i < gLights.size(); i++) {
    //glUniform1f(lightHandles.uLightAngleCos, cos(gLights.at(i).angle));
    /*glm::mat4 trans = glm::translate(glm::mat4(1.0f), gLights.at(i).position);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(10, 25, 10));
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), 80.0f, glm::vec3(1, 0, 0));
    glm::mat4 com = trans * scale * rot;*/
    /*safe_glUniformMatrix4fv(lightHandles.uModel, glm::value_ptr(com));
    safe_glUniformMatrix4fv(lightHandles.uProj, glm::value_ptr(viewCam->getProjection()));
    safe_glUniformMatrix4fv(lightHandles.uView, glm::value_ptr(viewCam->getView()));*/
    /*safe_glUniformMatrix4fv(lightHandles.uModel, glm::value_ptr(glm::mat4(1.0f)));
    safe_glUniformMatrix4fv(lightHandles.uProj, glm::value_ptr(glm::mat4(1.0f)));
    safe_glUniformMatrix4fv(lightHandles.uView, glm::value_ptr(glm::mat4(1.0f)));*/
    //lightHandles.draw(&coneMesh);
  //}

  checkGLError();
}

void endLightPass() {
  glActiveTextureARB(GL_TEXTURE0_ARB);
  glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTextureARB(GL_TEXTURE1_ARB);
  glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  glActiveTextureARB(GL_TEXTURE2_ARB);
  glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glUseProgram(0);
  checkGLError();
}

/*void endPass1Draw() {
  GLSL::disableVertexAttribArray(pass1Handles.aPosition);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}*/

void endDrawGL() {

  GLSL::disableVertexAttribArray(geomHandles.aPosition);
  GLSL::disableVertexAttribArray(geomHandles.aNormal);
  GLSL::disableVertexAttribArray(geomHandles.aTexCoord);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glUseProgram(0);
  checkGLError();
}

void window_size_callback(GLFWwindow* window, int w, int h){
  //glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  g_width = w;
  g_height = h;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
  g_framebuffer_width = width;
  g_framebuffer_height = height;
}

void endIntro()
{
  inIntro = false;
  drawCam = camera3DPerson;
  viewCam = camera3DPerson;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_N && (action == GLFW_PRESS) && shiftDown) {
    debug = !debug;
    if (debug) {
      viewCam = debugCam;
    }
    else {
      viewCam = drawCam;
    }
  }
  if (key == GLFW_KEY_B && (action == GLFW_PRESS) && shiftDown) {
    boxes = !boxes;
  }
  if (key == GLFW_KEY_H && (action == GLFW_PRESS) && shiftDown) {
    //soundObj->backGroundSnd->setIsPaused(false);
    endIntro();
    playerObject->handleOpenSounds(false);
    //soundObj->openingSnd->setIsPaused(true);
  }

  if (key == GLFW_KEY_R && action == GLFW_PRESS && debug) {
    curveOutput << debugCam->eye.x << " " << debugCam->eye.y << " " << debugCam->eye.z << endl;
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
  double x_center = g_framebuffer_width / 2.0;
  double y_center = g_framebuffer_height / 2.0;

  double dx = xpos - x_center;
  double dy = ypos - y_center;

  float maxMove = camSpeed * deltaTime;
  if (dx > 0) {
    dx = dx < maxMove ? dx : maxMove;
  }
  else {
    dx = dx > -1.0 * maxMove ? dx : -1.0 * maxMove;
  }
  if (dy > 0) {
    dy = dy < maxMove ? dy : maxMove;
  }
  else {
    dy = dy > -1.0 * maxMove ? dy : -1.0 * maxMove;
  }
  if (!inIntro) {
    viewCam->moveHoriz(-1.0 * dx * 0.01);
    viewCam->moveVert(dy * 0.01);
  }

  glfwSetCursorPos(window, x_center, y_center);
}

void initObjects(WorldGrid* gameObjects) {
  int levelDesign[TEST_WORLD][TEST_WORLD];
  int rot;

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
      char design = '0' + levelDesign[i][j];
      switch(design) {
      case '3': //barrels
        //printf("case 3\n");
        gameObjects->add(shared_ptr<GameObject>(new GameObject(
                &barrel,
                vec3(i - (TEST_WORLD/2), 0.3f, j - (TEST_WORLD/2)),
                vec3(1, 1.3, 1),
                getRand(0, 360),
                vec3(cos(getRand(0, 360) * M_PI / 180), 0, sin(getRand(0, 360) * M_PI / 180)), // direction
                0,
                vec3(1.5, 2.5, 1.5),
                1,
                0,
                GameObject::ObjectType::STATIC
                )));
        break;
      case '4': //stack of boxes
        //printf("case 4\n");
        rot = getRand(0, 3);
        gameObjects->add(shared_ptr<GameObject>(new GameObject(
                &boxStackMesh,
                vec3(i - (TEST_WORLD/2), 1, j - (TEST_WORLD/2)),
                vec3(3.5, 2, 3.5),
                rot * 90,
                vec3(cos(rot * M_PI / 2), 0, sin(rot * M_PI / 2)), // direction
                0,
                vec3(3.7, 3.7, 3.7),
                1,
                7,
                GameObject::ObjectType::STATIC
                )));
        break;
      case '5': //table
        //printf("case 5\n");
        gameObjects->add(shared_ptr<GameObject>(new GameObject(
                &tableMesh,
                vec3(i - (TEST_WORLD/2), -0.50, j - (TEST_WORLD/2)),
                vec3(1.5, 2, 1.5),
                0,
                vec3(1, 0, 0), // direction
                0,
                vec3(2.8, 1.5, 1.4),
                1,
                5,
                GameObject::ObjectType::PUSHABLE
                )));
        break;
      case '6': //chair
        //printf("case 6\n");
        gameObjects->add(shared_ptr<GameObject>(new GameObject(
                &chairMesh,
                vec3(i - (TEST_WORLD/2), 0, j - (TEST_WORLD/2)),
                vec3(0.75, 0.75, 0.75),
                getRand(0, 360),
                vec3(cos(getRand(0, 360) * M_PI / 180), 0, sin(getRand(0, 360) * M_PI / 180)), // direction
                0,
                vec3(1.5, 2, 1.5),
                1,
                8,
                GameObject::ObjectType::PUSHABLE
                )));
        break;
      case '7': 
        //add an object here!
        break;
      case '8': //rafter
        //printf("case 8\n");
        gameObjects->add(shared_ptr<GameObject>(new GameObject(
                &rafterMesh,
                vec3(i - (TEST_WORLD/2), 16, j - (TEST_WORLD/2)),
                vec3(24, 15, 24),
                90,
                vec3(cos(0.5 * M_PI), 0, sin(0.5 * M_PI)), // direction
                0,
                vec3(44, 5.0, 1.0),
                1,
                6,
                GameObject::ObjectType::STATIC
                )));
        break;
      case '9': //blob
        //printf("case 9\n");
        gameObjects->add(shared_ptr<GameObject>(new WinCondition(
                &winMesh,
                vec3(i - (TEST_WORLD/2), -0.8f, j - (TEST_WORLD/2)),
                vec3(2.5f, 4, 2.5f),
                0,
                vec3(0, 0, 1), // direction
                0,
                vec3(5, 1, 5),
                1,
                3
                )));
        break;
      case 'T': //pedestal
        //printf("case 9\n");
        gameObjects->add(shared_ptr<GameObject>(new GameObject(
                &trainMesh,
                vec3(i - (TEST_WORLD/2), 1, j - (TEST_WORLD/2)),
                vec3(8, 8, 8),
                90,
                vec3(0, 0, 1), // direction
                0,
                vec3(4, 4, 14),
                1,
                3,
                GameObject::ObjectType::STATIC
                )));
        break;
      case 'L': {
                  //glm::vec3 light((1024 * i) / TEST_WORLD, 15.0, (780 * j) / TEST_WORLD);
                  //printf("case 'L'\n");
                  //printf("light position %lf %lf %lf\n", i - (TEST_WORLD / 2.0), (float)15.0, j - (TEST_WORLD / 2.0));
                  Light spotLight;
				  spotLight.guardLight = false;
                  spotLight.position = glm::vec3(i - (TEST_WORLD / 2), 15.0, j - (TEST_WORLD / 2));
				  gameObjects->add(shared_ptr<GameObject>(new DetailProp(
					  &lightMesh,
					  spotLight.position + vec3(0, 1.3, 0),
					  vec3(3, 2.0f, 3),
					  0,
					  vec3(0, 0, 1),
					  0,
					  vec3(3, 8, 3),
					  9
					  )));
                  //printf("i: %d, j: %d\n", i, j);
                  //printf("spotlight position %lf %lf %lf\n", spotLight.position.x, spotLight.position.y, spotLight.position.z);
                  /*spotLight.atten = glm::vec3(0.0, 0.0001, 0.0002);
                  spotLight.color = glm::vec3(1.0, 1.0, 1.0);
                  spotLight.angle = 15.0f;
                  spotLight.direction = glm::vec3(0.0, -1.0, 0.0);*/
                  spotLight.direction = glm::vec3(0.0, -1.0, 0.0);
                  gLights.push_back(spotLight);
                  //lights.push_back(glm::vec3( i - (TEST_WORLD / 2.0), 15.0, j - (TEST_WORLD / 2.0)));
                  break;
                }
      case '!': {
        gameObjects->add(shared_ptr<GameObject>(new Clue(
                &clueMesh,
                vec3(i - (TEST_WORLD/2), 1, j - (TEST_WORLD/2)),
                vec3(0, 0, 0),
                vec3(1, 1, 1),
                vec3(0, 0, 1), // direction
                0,
                vec3(1, 2, 1),
                1,
                3,
                "../dependencies/irrKlang/media/collect0.wav"
                )));
                break;
      }
      case '@': {
        gameObjects->add(shared_ptr<GameObject>(new Clue(
                &clueMesh,
                vec3(i - (TEST_WORLD/2), 1, j - (TEST_WORLD/2)),
                vec3(-64.59f, 0, 95.87f),
                vec3(1, 1, 1),
                vec3(0, 0, 1), // direction
                0,
                vec3(1, 2, 1),
                1,
                3,
                "../dependencies/irrKlang/media/collect1.wav"
                )));
              break;
      }
      case '#': {
        gameObjects->add(shared_ptr<GameObject>(new Clue(
                &clueMesh,
                vec3(i - (TEST_WORLD/2), 1, j - (TEST_WORLD/2)),
                vec3(-47.0f, 0, -3.0f),
                vec3(1, 1, 1),
                vec3(0, 0, 1), // direction
                0,
                vec3(1, 2, 1),
                1,
                3,
                "../dependencies/irrKlang/media/collect2.wav"
                )));
              break;
      }
      case '$': {
        gameObjects->add(shared_ptr<GameObject>(new Clue(
                &clueMesh,
                vec3(i - (TEST_WORLD/2), 1, j - (TEST_WORLD/2)),
                vec3(-91.5f, 0, 85.0f),
                vec3(1, 1, 1),
                vec3(0, 0, 1), // direction
                0,
                vec3(1, 2, 1),
                1,
                3,
                "../dependencies/irrKlang/media/collect3.wav"
                )));
              break;
	case 'f':
	  gameObjects->add(shared_ptr<GameObject>(new DetailProp(
								 &printMesh,
								 vec3(i - (TEST_WORLD/2), -1, j - (TEST_WORLD/2)),
								 vec3(0.5, 0.1, 0.5),
								 0,
								 vec3(0, 0, 1),
								 0,
								 vec3(1, 1, 1),
								 9
								 )));
	  break;
	case 'g':
	  gameObjects->add(shared_ptr<GameObject>(new DetailProp(
                                                                 &printMesh,
                                                                 vec3(i - (TEST_WORLD/2), -1, j - (TEST_WORLD/2)),
                                                                 vec3(0.5, 0.1, 0.5),
                                                                 -90,
                                                                 vec3(0, 0, 1),
                                                                 0,
		 vec3(1, 1, 1),
		 9
		 )));
	  break;
	case 'h':
	  gameObjects->add(shared_ptr<GameObject>(new DetailProp(
                                                                 &printMesh,
                                                                 vec3(i - (TEST_WORLD/2), -1, j - (TEST_WORLD/2)),
                                                                 vec3(0.5, 0.1, 0.5),
                                                                 180,
                                                                 vec3(0, 0, 1),
                                                                 0,
		 vec3(1, 1, 1),
		 9
		 )));
	  break;
      }
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
      vec3(1.0, 1.0, 1.0), //scale
      vec3(cos(0.11 * M_PI), 0, sin(0.11 * M_PI)),      // direction
      vec3(1.0, 2.0, 1.0),
      1,
      0
      );

  gameObjects->add(shared_ptr<GameObject>(playerObject));
}
#define GUARD_Y_SHIFT 1.0f
void initGuards(WorldGrid* gameObjects) {
  vector<PathNode> guardPath;
  FILE *file = fopen(resPath("GuardPaths.txt").data(), "r");
  char line[100];
  float x, y, z, dur;
  char smartTurn, endTurnDir;
  int numNodes;
  int guardNum = 0;

  vector<char*> reactSnds;
  reactSnds.push_back("../dependencies/irrKlang/media/guardRand1_Reac.wav");
  reactSnds.push_back("../dependencies/irrKlang/media/guardRand3_Reac.wav");
  reactSnds.push_back("../dependencies/irrKlang/media/guardRand4_Reac.wav");
  reactSnds.push_back("../dependencies/irrKlang/media/guardRand5_Reac.wav");
  reactSnds.push_back("../dependencies/irrKlang/media/guardRand6_Reac.wav");

  vector<char*> dismissSnds;
  dismissSnds.push_back("../dependencies/irrKlang/media/guardRand1_Dismiss.wav");
  dismissSnds.push_back("../dependencies/irrKlang/media/guardRand3_Dismiss.wav");
  dismissSnds.push_back("../dependencies/irrKlang/media/guardRand4_Dismiss.wav");
  dismissSnds.push_back("../dependencies/irrKlang/media/guardRand5_Dismiss.wav");
  int reactIndx, dismissIndx;
  reactIndx = dismissIndx = 0;

  while (fgets(line, 100, file)) {
    if (line[0] == 'G') { // build new guard
      guardPath.clear();
      fgets(line, 100, file); // guard settings, ignored for now
      fscanf(file, "%s %d", line, &numNodes); // read number of nodes in path

      for (int i = 0; i < numNodes; i++) { // read in numNodes nodes
        fscanf(file, "%f %f %f %c %f %c", &x, &y, &z, &smartTurn, &dur, &endTurnDir);
        y += GUARD_Y_SHIFT;
        //printf("NODE: %f %f %f %c %f %c\n", x, y, z, smartTurn, dur, endTurnDir);
        guardPath.push_back(PathNode(vec3(x, y, z), smartTurn == 'y', dur, endTurnDir == 'r', endTurnDir != 'x'));
      }


	  Light spotLight;
	  spotLight.atten = glm::vec3(0.0, 0.0001, 0.0002);
	  spotLight.color = glm::vec3(1.0, 1.0, 1.0);
	  spotLight.angle = 15.0f;
	  spotLight.guardLight = true;
	  gLights.push_back(spotLight);

      if (guardNum == 0) {
      Guard* guardObject = new Guard(
          &guardMesh,
          vec3(2, 2, 2),
          GUARD_SPEED,
          vec3(2, 4, 2),
          1,
          0,
          guardPath,
		  playerObject,
		  &gLights,
		  gLights.size() - 1,
          "../dependencies/irrKlang/media/guard1_Reac.wav",
		  "../dependencies/irrKlang/media/guard1_Dismiss.wav"
          );
        gameObjects->add(shared_ptr<GameObject>(guardObject));
      }
      else if (guardNum == 1) {
        Guard* guardObject = new Guard(
          &guardMesh,
          vec3(2, 2, 2),
          GUARD_SPEED,
          vec3(2, 4, 2),
          1,
          0,
          guardPath,
		  playerObject,
		  &gLights,
		  gLights.size() - 1,
          "../dependencies/irrKlang/media/guard2_Reac.wav",
		  "../dependencies/irrKlang/media/guard2_Dismiss.wav"
          );
        gameObjects->add(shared_ptr<GameObject>(guardObject));
      }
      else {
      Guard* guardObject = new Guard(
          &guardMesh,
          vec3(2, 2, 2),
          GUARD_SPEED,
          vec3(2, 4, 2),
          1,
          0,
          guardPath,
		  playerObject,
		  &gLights,
		  gLights.size() - 1,
          reactSnds[reactIndx],
          dismissSnds[dismissIndx]
          );
      gameObjects->add(shared_ptr<GameObject>(guardObject));

    }
    }
    reactIndx++;
    dismissIndx++;
    guardNum++;
    if (reactIndx == reactSnds.size())
      reactIndx = 0;
    if (dismissIndx == dismissSnds.size())
      dismissIndx = 0;
  }
}

void initGround() {
  ground = new Shape(
      vec3(0), //position
      vec3(5, 1, 5), //scale
      vec3(0, 0, 1), //direction
      0, //velocity
      6, //indices
      posBufObjG,
      norBufObjG,
      idxBufObjG,
      texBufObjG,
      1 //material
      );
  // printf("loading concrete.bmp for ground\n");
  ground->loadMipmapTexture(resPath(sysPath("textures", "ground.bmp")), 512);
}

void initCeiling() {
  ceiling = new Shape(
      vec3(0, 20, 0), //position
      vec3(5, 1, 5), //scale
      vec3(0, 0, 1), //direction
      0, //velocity
      6, //indices
      posBufObjG,
      norBufObjG,
      5 //material
      );

  ceiling->loadMipmapTexture(resPath(sysPath("textures", "wall.bmp")), 512);
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
                                                                     &shortCubeMesh,
                                                                     vec3(center.x, -0.7f, center.y),      //position
                                                                     vec3(dims.x / 2, 1.0f, dims.y / 2),    //scale
                                                                     vec3(0, 0, 1),  //direction (this value is important to setting model matrix)
                                                                     0,
                                                                     vec3(dims.x, 2.0f, dims.y),     //dimensions
                                                                     0,            //scanRadius
                                                                     6             //material
                                                                     )));
                    shortWall = false;     
                }
                else {
                    gameObjects->add(shared_ptr<GameObject>(new Wall(
                                                                     &cubeMesh,
                                                                     vec3(center.x, 9, center.y),      //position
                                                                     vec3(dims.x / 2, 10, dims.y / 2),    //scale
                                                                     vec3(0, 0, 1),  //direction (this value is important to setting model matrix)
                                                                     0,
                                                                     vec3(dims.x, 20, dims.y),     //dimensions
                                                                     0,            //scanRadius
                                                                     4             //material
                                                                     )));
                }
                testWallCount++;
                //printf("\nCenter point of testWall: %d,  (x: %f, z: %f)\n", testWallCount, tempPos.x, tempPos.z);
            }
        }
        testWallCount++;
        //printf("\nCenter point of testWall: %d,  (x: %f, z: %f)\n", testWallCount, tempPos.x, tempPos.z);
      }
    }
  }

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
    g_width = 1280;
    g_height = 720;
    printf("%d, %d\n", g_width, g_height);
    window = glfwCreateWindow(g_width, g_height, "Hear No Evil, See No Evil", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
    depthHandles.installShaders(resPath(sysPath("shaders", "depthVert.glsl")), resPath(sysPath("shaders", "depthFrag.glsl")));
    //pass2Handles.installShaders(resPath(sysPath("shaders", "pass2Vert.glsl")), resPath(sysPath("shaders", "pass2Frag.glsl")));
    lightHandles.installShaders(resPath(sysPath("shaders", "lightVert.glsl")), resPath(sysPath("shaders", "lightFrag.glsl")));
    geomHandles.installShaders(resPath(sysPath("shaders", "geometryVert.glsl")), resPath(sysPath("shaders", "geometryFrag.glsl")));
    //coneMesh.loadShapes(resPath(sysPath("models", "cone.obj")));
    assert(glGetError() == GL_NO_ERROR);
    printMesh.loadShapes(resPath(sysPath("models", "shoe-male.obj")));
    clueMesh.loadShapes(resPath(sysPath("models", "magnifying-glass.obj")));
    clueMesh.hasTexture = true;
    clueMesh.loadClueMipmapTexture(resPath(sysPath("textures", "m_glass.bmp")), 512);
    trainMesh.loadShapes(resPath(sysPath("models", "train.obj")));
    trainMesh.hasTexture = true;
    trainMesh.loadMipmapTexture(resPath(sysPath("textures", "train.bmp")), TEX_SIZE);
    guardMesh.loadShapes(resPath(sysPath("models", "guard2.obj")));
    playerMesh.loadShapes(resPath(sysPath("models", "player.obj")));
    cubeMesh.loadShapes(resPath(sysPath("models", "cube.obj")));
    shortCubeMesh.loadShapes(resPath(sysPath("models", "cube.obj")));
    barrel.loadShapes(resPath(sysPath("models", "barrel.obj")));
    boxStackMesh.loadShapes(resPath(sysPath("models", "crate.obj")));
    tableMesh.loadShapes(resPath(sysPath("models", "desk.obj")));
	lightMesh.loadShapes(resPath(sysPath("models", "ceilingLight.obj")));
    chairMesh.hasTexture = true;
    chairMesh.loadShapes(resPath(sysPath("models", "chair.obj")));
    chairMesh.loadMipmapTexture(resPath(sysPath("textures", "chair.bmp")), TEX_SIZE);
    rafterMesh.loadShapes(resPath(sysPath("models", "rafter.obj")));
    winMesh.loadShapes(resPath(sysPath("models", "blob.obj")));
    winMesh.hasTexture = true;
    winMesh.loadMipmapTexture(resPath(sysPath("textures", "blob.bmp")), TEX_SIZE);
    playerMesh.hasTexture = true;
    playerMesh.loadMipmapTexture(resPath(sysPath("textures", "player_texture.bmp")), TEX_SIZE);
	lightMesh.hasTexture = true;
	lightMesh.loadMipmapTexture(resPath(sysPath("textures", "light.bmp")), 512);
    //printf("Loading cube mesh wall.bmp\n");
    cubeMesh.sendWallTexBuf();
    cubeMesh.loadMipmapTexture(resPath(sysPath("textures", "wall.bmp")), 2048);
    shortCubeMesh.sendWallTexBuf();
    shortCubeMesh.loadMipmapTexture(resPath(sysPath("textures", "shortwall.bmp")), 512);
    cubeMesh.hasTexture = true;
    shortCubeMesh.hasTexture = true;
    tableMesh.hasTexture = true;
    tableMesh.loadMipmapTexture(resPath(sysPath("textures", "desk.bmp")), TEX_SIZE);
    barrel.hasTexture = true;
	printMesh.hasTexture = true;
	printMesh.loadMipmapTexture(resPath(sysPath("textures", "shoe.bmp")), 1);
    barrel.loadTexture(resPath(sysPath("textures", "barrel.bmp")));
    //cubeMesh.loadTexture(resPath(sysPath("textures", "wall.bmp")));
    boxStackMesh.hasTexture = true;
    boxStackMesh.loadMipmapTexture(resPath(sysPath("textures", "crate.bmp")), TEX_SIZE);
    guardMesh.hasTexture = true;
    guardMesh.loadMipmapTexture(resPath(sysPath("textures", "guard.bmp")), TEX_SIZE);
    
    srand(time(NULL));
    
    //initialize game objects
    //vector<shared_ptr <GameObject> > gameObjects;
    //First item is always the player, followed by numGuards guards,
    //	followed by however many walls we need. -JB
    WorldGrid gameObjects(WORLD_WIDTH, WORLD_HEIGHT);
    detecTrac = new DetectionTracker();

	//gLights = new vector<Light>();
    
    initPlayer(&gameObjects);
    initGuards(&gameObjects);
    initObjects(&gameObjects);
    initWalls(&gameObjects);
    initGround();
    initCeiling();
    //initDetectionTracker(&gameObjects);
    //initFramebuffer();
    glfwGetFramebufferSize(window, &g_framebuffer_width, &g_framebuffer_height);
    m_dbuffer.init(1024, 1024);
    m_gbuffer.Init(g_framebuffer_width, g_framebuffer_height);
   
    readIntroSpline();

    //initialize the camera
    camera3DPerson = new Camera3DPerson(&gameObjects, playerObject, CAMERA_ZOOM, CAMERA_FOV,
                                        (float)g_width / (float)g_height,
                                        CAMERA_NEAR, CAMERA_FAR, debugDraw);
    // debug camera
    debugCam = new Camera(glm::vec3(1.0f, 1.0f, 1.0f),
                             glm::vec3(0.0f, 1.0f, 0.0f),
                             glm::vec3(0.0f, 1.0f, 0.0f),
                             CAMERA_FOV,
                             (float)g_width / (float)g_height,
                             CAMERA_NEAR,
                             CAMERA_FAR,
                             debugDraw);

    detectCam = new DetectionCamera(GUARD_FOV,
                                    (float)g_width / (float)g_height,
                                    CAMERA_NEAR,
                                    GUARD_FAR,
                                    debugDraw);

    // CAREFUL - can only call getLocation if introCurve has data in it already!
    cineCam = new Camera(introCurve.getLocation(0.1),
                             introCurve.getLocation(0),
                             glm::vec3(0.0f, 1.0f, 0.0f),
                             CAMERA_FOV,
                             (float)g_width / (float)g_height,
                             CAMERA_NEAR,
                             CAMERA_FAR,
                             debugDraw);

    drawCam = camera3DPerson;
    viewCam = camera3DPerson;

#ifdef DEBUG
    curveOutput.open(resPath("introCurveSample.txt"));
#endif
    
    double timeCounter = 0;
    
    //  printf("shadow map id: %d\n", shadowMap);
    //printf("player tex id: %d\n", playerMesh.texId);

    ImGui_ImplGlfw_Init(window, false);
    ImGuiIO& io = ImGui::GetIO();

    do{
        ImGui_ImplGlfw_NewFrame(); 

        //timer stuff
        TimeManager::Instance().CalculateFrameRate(true);
        deltaTime = TimeManager::Instance().DeltaTime;
        double currentTime = TimeManager::Instance().CurrentTime;
        timeCounter += deltaTime;
        
        bool open = true;

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 255.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1, 1, 1, 0));
        if (detecTrac->totalDetLvl < 0.4) {
          ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1, 1, 1, 1));
        }
        else {
          ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(detecTrac->totalDetLvl, 0, 0, 1));
        }
        ImGui::Begin("Detection", &open, ImGuiWindowFlags_NoTitleBar 
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowSize(ImVec2(io.DisplaySize.x * 1.52, io.DisplaySize.y / 7));
        ImGui::SetWindowPos(ImVec2(0, 10));
        ImGui::SetWindowFontScale(3.5f);
        ImGui::SliderFloat("", &detecTrac->totalDetLvl, 0.0f, 1.0f);
        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

#ifdef DEBUG
        if (boxes) {
          double iter = 0;
          glm::vec3 oldPoint = introCurve.getLocation(0);
          glm::vec3 newPoint;
          while (iter < introCurve.getMaxDist()) {
            newPoint = introCurve.getLocation(iter);
            debugDraw->addLine(oldPoint, newPoint, glm::vec3(0.87f, 0.25f, 0.44f), false);
            oldPoint = newPoint;
            iter += 0.01;
          }
        }
#endif

        if (introDist < introCurve.getMaxDist() && inIntro) {
          //soundObj->openingSnd->setIsPaused(false);
          playerObject->handleOpenSounds(true);
          drawCam = cineCam;
          if (debug) {
            viewCam = debugCam;
          }
          else {
            viewCam = cineCam;
          }
          glm::vec3 nextPoint = introCurve.getLocation(introDist);
          cineCam->eye = cineCam->lookat;
          cineCam->lookat = nextPoint;
          /*if (introDist + 5 < introCurve.getMaxDist()) {
            cineCam->lookat = introCurve.getLocation(introDist + 5);
          }
          else {
            cineCam->lookat = introCurve.getLocation(introCurve.getMaxDist() - 0.01);
          }*/

          //target intro time is 1:20
          introDist += deltaTime * introCurve.getMaxDist() / introTime;
        }
        else if (inIntro) {
          endIntro();
          playerObject->handleOpenSounds(false);
          //soundObj->openingSnd->setIsPaused(true);
          //soundObj->backGroundSnd->setIsPaused(false);
        }
        
        camera3DPerson->update();
	//for (int i = 0; i < gLights.size(); i++) {
	/*beginPass1Draw();
	  drawPass1(&gameObjects);
	  endPass1Draw();
	  beginPass2Draw();
	  getWindowInput(window, deltaTime);
	  drawGameObjects(&gameObjects, deltaTime);
	  endDrawGL();*/
        //}
        shadowPass(&gameObjects);
        getWindowInput(window, deltaTime);
	geometryPass(&gameObjects, deltaTime);
	  endDrawGL();
	//	beginLightPass();
        if (!inIntro)
          ImGui::Render();

	lightPass();
	endLightPass();
        
        // draw debug
        if (debug || boxes) {
            if (debug) {
                // just to trigger geometry being added to debug drawer
                camera3DPerson->getView();
                camera3DPerson->getProjection();
            }

            debugDraw->view = viewCam->getView();
            debugDraw->projection = viewCam->getProjection();
            
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
//#ifndef WIN32
      debugDraw->drawAll();
//#endif
    }
    debugDraw->clear();

    glfwSwapBuffers(window);
    glfwPollEvents();
    //printf("curr pos %f, %f, %f\n", playerObject->position.x, playerObject->position.y, playerObject->position.z);
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
      && glfwWindowShouldClose(window) == 0);

#ifdef DEBUG
    curveOutput.close();
#endif
  glfwTerminate();
  soundObj->cleanUpSound();
  return 0;
}
