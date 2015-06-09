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
	genTexCoords(0.5f);
	sendTexCoords();
}

void Wall::genTexCoords(float mult) {	// incorporate mult!!!!!!
	// 1 - face 1 (front) 1 2 3 4
	texBuf.push_back(0.5f - dimensions.x * 0.125f * mult); texBuf.push_back(0.0f);
	// 2
	texBuf.push_back(0.5f + dimensions.x * 0.125f * mult); texBuf.push_back(0.0f);
	// 3
	texBuf.push_back(0.5f - dimensions.x * 0.125f * mult); texBuf.push_back(dimensions.y * 0.25f * mult);
	// 4
	texBuf.push_back(0.5f + dimensions.x * 0.125f * mult); texBuf.push_back(dimensions.y * 0.25f * mult);

	// face 2 (top) doesn't matter what the texture is, we never see it
	texBuf.push_back(0); texBuf.push_back(0);
	texBuf.push_back(0); texBuf.push_back(0);
	texBuf.push_back(0); texBuf.push_back(0);
	texBuf.push_back(0); texBuf.push_back(0);

	// 9 - face 3 (back) 8 7 6 5
	texBuf.push_back(texBuf[6]); texBuf.push_back(texBuf[7]);
	// 10
	texBuf.push_back(texBuf[4]); texBuf.push_back(texBuf[5]);
	// 11
	texBuf.push_back(texBuf[2]); texBuf.push_back(texBuf[3]);
	// 12
	texBuf.push_back(texBuf[0]); texBuf.push_back(texBuf[1]);

	// face 4 (bottom) doesn't matter what the texture is, we never see it
	texBuf.push_back(0); texBuf.push_back(0);
	texBuf.push_back(0); texBuf.push_back(0);
	texBuf.push_back(0); texBuf.push_back(0);
	texBuf.push_back(0); texBuf.push_back(0);

	// 17 - face 5 (right) 13 1 14 3
	texBuf.push_back(texBuf[0] - dimensions.z * 0.25f * mult); texBuf.push_back(0);
	// 18
	texBuf.push_back(texBuf[0]); texBuf.push_back(0);
	// 19
	texBuf.push_back(texBuf[32]); texBuf.push_back(texBuf[5]);
	// 20
	texBuf.push_back(texBuf[4]); texBuf.push_back(texBuf[5]);

	// 21 - face 6 (left) 2 11 4 12
	texBuf.push_back(texBuf[2]); texBuf.push_back(0);
	// 22
	texBuf.push_back(texBuf[2] - dimensions.z * 0.25f * mult); texBuf.push_back(0);
	// 23
	texBuf.push_back(texBuf[6]); texBuf.push_back(texBuf[7]);
	// 24
	texBuf.push_back(texBuf[42]); texBuf.push_back(texBuf[7]);
}

void Wall::sendTexCoords() {
	glGenBuffers(1, &texBufObj);
	glBindBuffer(GL_ARRAY_BUFFER, texBufObj);
	glBufferData(GL_ARRAY_BUFFER, texBuf.size() * sizeof(float), &texBuf[0], GL_STATIC_DRAW);
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
