#include "Wall.h"
#include "Player.h"
#include "../WorldGrid/WorldGrid.h"

Wall::Wall(Mesh *mesh, Handles *handles,
  vec3 position, float rotation, vec3 scale,
  vec3 direction, float velocity, vec3 dimensions, 
  int scanRadius, int material = 0) :
  GameObject(mesh, handles, position, rotation, scale,
  direction, velocity, dimensions, scanRadius, material) {

}

void Wall::move(float time) {
  if (position.x >= 29.0f) {
    position.x = 29.0f;
    direction.x = -direction.x;
  }
  if (position.x <= -29.0f) {
    position.x = -29.0f;
    direction.x = -direction.x;
  }
  if (position.z >= 29.0f) {
    position.z = 29.0f;
    direction.z = -direction.z;
  }
  if (position.z <= -29.0f) {
    position.z = -29.0f;
    direction.z = -direction.z;
  }

  position += normalize(direction) * velocity * time;
  rotation = 90 + (atan2f(direction.x, direction.z) * 180 / M_PI);
}

bool Wall::collide(GameObject* object) {
  float thisRadius = dimensions.x + dimensions.y + dimensions.z;
  float objectRadius = object->dimensions.x + object->dimensions.y +
    object->dimensions.z;

  if (compareDistance(position, object->position, thisRadius + objectRadius)) {
    if (intersect(position.x, object->position.x, dimensions.x, object->dimensions.x) &&
      intersect(position.y, object->position.y, dimensions.y, object->dimensions.y) &&
      intersect(position.z, object->position.z, dimensions.z, object->dimensions.z)) {
      return true;
    }
  }
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
