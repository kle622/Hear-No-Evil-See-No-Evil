#include "Wall.h"
#include "Player.h"
#include "../WorldGrid/WorldGrid.h"

Wall::Wall(Mesh *mesh,
  vec3 position, vec3 scale,
  vec3 direction, float velocity, vec3 dimensions, 
  int scanRadius, int material = 0) :
  GameObject(mesh, position, scale, 0,
  direction, velocity, dimensions, scanRadius, material, ObjectType::STATIC) {
	texBuf = std::vector<float>();
	genTexCoords();
}

void Wall::genTexCoords() {
	// coord 1
	texBuf.push_back(0.5f - dimensions.x * 0.125f);
	texBuf.push_back(0.0f);
	// coord 2
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 3
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 4
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 5
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 6
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 7
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 8
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 9
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 10
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 11
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 12
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 13
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
	// coord 14
	texBuf.push_back(0.0f);
	texBuf.push_back(0.0f);
}

void Wall::move(float time) {
}

bool Wall::collide(GameObject* object) {
  return false;
}

//void Wall::initWalls(WorldGrid* gameobjects) {
//    int levelDesign[WORLD_WIDTH][WORLD_HEIGHT];
//      char ch;
//      fstream fin(resPath("LevelDesign.txt"), fstream::in);
//      int i = 0, j = 0;
//      while (fin >> noskipws >> ch) {
//        if (ch == '\n') {
//          j = 0;
//          i++;
//        }
//        else {
//          levelDesign[i][j] = ch - '0';
//          j++;
//        }
//      }
//      // Test Print
//      for (i = 0; i < WORLD_WIDTH; i++) {
//        cout << '\n';
//        for (j = 0; j < WORLD_HEIGHT; j++) {
//          cout << levelDesign[i][j];
//        }
//      }
//      cout << '\n';
//      // Create the wall objects
//      for (i = 0; i < 50; i++) {
//        for (j = 0; j < 50; j++) {
//          if (levelDesign[i][j]) {
//            gameObjects->add(shared_ptr<GameObject>(new Wall(
//          &cubeMesh,
//          &handles, //model handle
//              vec3((i-25), 4, (j-25)), //position
//              0, //rotation
//              vec3(1.0, 5.0, 1.0), //scale
//              vec3(1, 0, 0), //direction
//              0, //speed
//              vec3(1, 8, 1), //bounding box
//              0, //scanRadius
//              1  //material
//              )));
//          }
//        }
//      }
//  }
//}
