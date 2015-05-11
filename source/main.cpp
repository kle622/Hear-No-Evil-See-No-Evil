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
#include "GameObject/Mesh.h"
#include "Camera/Camera.h"
#include "Camera/Camera3DPerson.h"
#include "Path/Path.h"
#include "WorldGrid/WorldGrid.h"
//#include "GuardPath/PathNode.h"

//#include "openAL/include/AL/al.h"
//#include "openAL/include/AL/alc.h"
//#include "openAL/include/includeAllOpenAL.h"

#define WORLD_WIDTH 300
#define WORLD_HEIGHT 300
#define TEST_WORLD 200

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
Shape *ground;
Shape *ceiling;
bool debug = false;
bool boxes = false;
DebugDraw debugDraw;

glm::vec3 g_light(0, 100, 0);

GLuint posBufObjG = 0;
GLuint norBufObjG = 0;

double deltaTime;

// OpenAL error catching
int endWithError(char* msg, int error = 0)
{
  //Display error message in console
  cout << msg << "\n";
  system("PAUSE");
  return error;
}


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
    glUniform3f(mainShader.uMatAmb, 0.05f, 0.025f, 0.025f);
    glUniform3f(mainShader.uMatDif, 0.9f, 0.1f, 0.05f);
    glUniform3f(mainShader.uMatSpec, 0.8f, 0.2f, 0.2f);
    glUniform1f(mainShader.uMatShine, 100.0f);
    break;
  case 1: // grey (people + arms)
    glUniform3f(mainShader.uMatAmb, 0.13f, 0.13f, 0.14f);
    glUniform3f(mainShader.uMatDif, 0.3f, 0.3f, 0.4f);
    glUniform3f(mainShader.uMatSpec, 0.3f, 0.3f, 0.4f);
    glUniform1f(mainShader.uMatShine, 150.0f);
    break;
  case 2: // white (bunnies)
    glUniform3f(mainShader.uMatAmb, 0.09f, 0.2f, 0.08f);
    glUniform3f(mainShader.uMatDif, 0.9f, 0.9f, 0.9f);
    glUniform3f(mainShader.uMatSpec, 1.0f, 0.95f, 0.85f);
    glUniform1f(mainShader.uMatShine, 400.0f);
    break;
  case 3: // green (ground)
    glUniform3f(mainShader.uMatAmb, 0.06f, 0.09f, 0.06f);
    glUniform3f(mainShader.uMatDif, 0.2f, 0.80f, 0.1f);
    glUniform3f(mainShader.uMatSpec, 0.8f, 1.0f, 0.8f);
    glUniform1f(mainShader.uMatShine, 4.0f);
    break;
  case 4: // black (hats)
    glUniform3f(mainShader.uMatAmb, 0.08f, 0.08f, 0.08f);
    glUniform3f(mainShader.uMatDif, 0.08f, 0.08f, 0.08f);
    glUniform3f(mainShader.uMatSpec, 0.08f, 0.08f, 0.08f);
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
    }

    if (accelerate) {
      direction = normalize(direction);
      if ((upD && downD) || (leftD && rightD)) {
        playerObject->decelerate();
      }
      else {
        playerObject->changeDirection(direction);
        playerObject->accelerate();
        printf("velocity: %f\n", playerObject->velocity);
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
      for (int j = 0; j < gameObjects->wallList.size(); j++) {
        if (gameObjects->list[i]->collide(gameObjects->wallList[j].get())) {
          //collides with wall
        }
      }
    }

    //guards
    if (guard = dynamic_cast<Guard*>(gameObjects->list[i].get())) {
      guard->detect(playerObject);
    }
  }

  gameObjects->update();
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
      phi += dy * cursor_speed;
    }
    debugCamera->lookat.x = debugCamera->eye.x + cos(phi * M_PI / 180) * cos(theta * M_PI / 180);
    debugCamera->lookat.y = debugCamera->eye.y + sin(phi * M_PI / 180);
    debugCamera->lookat.z = debugCamera->eye.z + cos(phi * M_PI / 180) * sin(-1.0 * theta * M_PI / 180);
  }

  glfwSetCursorPos(window, x_center, y_center);
}

void initObjects(WorldGrid* gameObjects) {
  GameObject* tripleBarrel = new GameObject(
    &tripleBarrelMesh,
    &mainShader,
    vec3(5, 1, 0),
    0, 
    vec3(2.5, 2.5, 2.5),
    vec3(1.0, 0, 0),
    0,
    vec3(2.5, 4, 4.5),
    1,
    0
  );

  gameObjects->add(shared_ptr<GameObject>(tripleBarrel));

    GameObject* boxStack = new GameObject(
    &boxStackMesh,
    &mainShader,
    vec3(10, 1, 0),
    0, 
    vec3(4, 2, 4),
    vec3(1.0, 0.0, 0.0),
    0,
    vec3(3.0, 5, 3.0),
    1,
    0
  );

  gameObjects->add(shared_ptr<GameObject>(boxStack));
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
      0
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
      4 //material
      );
}

void initCeiling() {
  ceiling = new Shape(
      &mainShader, //model handle
      vec3(0, 10, 0), //position
      0, //rotation
      vec3(5, 1, 5), //scale
      vec3(1, 0, 0), //direction
      0, //velocity
      6, //indices
      posBufObjG, 
      norBufObjG,
      4 //material
      );
}

void initWalls(WorldGrid* gameObjects) {
	int levelDesign[TEST_WORLD][TEST_WORLD], tempI, tempJ, realI, realJ;
	float posX, posY;
	vec3 tempScale, tempPos, tempBBox;
	int testWallCount = 0;

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
	for (i = 0; i < TEST_WORLD; i++) {
		cout << '\n';
		for (j = 0; j < TEST_WORLD; j++) {
			cout << levelDesign[i][j];
    }
}
	cout << '\n';

	vec2 start, end;
	//////////// Create the wall objects
	for (i = 0; i < TEST_WORLD; i++) {
		for (j = 0; j < TEST_WORLD; j++) {
			if (levelDesign[i][j] == 1) {
				start = vec2(i, j);
				int i1 = i, j1 = j;
				// build wall depending on direction it is oriented
				if (i1 != TEST_WORLD - 1 && levelDesign[i1 + 1][j1] == 1) { // going right
					while (i1 < TEST_WORLD && levelDesign[i1][j1] == 1) {
						levelDesign[i1][j1] = 0;
						i1++;
					}
					end = vec2(i1 - 1, j1);
				}
				else if (j1 != TEST_WORLD - 1 && levelDesign[i1][j1 + 1] == 1) { // going down
					while (j1 < TEST_WORLD && levelDesign[i1][j1] == 1) {
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
				gameObjects->add(shared_ptr<GameObject>(new Wall(
					&cubeMesh,
					&mainShader,
					vec3(center.x, 1, center.y),      //position
					0,            //rotation
					vec3(dims.x / 2, 10, dims.y / 2),    //scale
					vec3(1, 0, 0),	//direction
					0,
					vec3(dims.x, 20, dims.y),     //dimensions
					0,            //scanRadius
					4             //material
					)));

				testWallCount++;
				printf("\nCenter point of testWall: %d,  (x: %f, z: %f)\n", testWallCount, tempPos.x, tempPos.z);
			}
		}
	}
}

int main(int argc, char **argv)
{


  ////////////////////////////////////////////////////

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

  return 0;
}
