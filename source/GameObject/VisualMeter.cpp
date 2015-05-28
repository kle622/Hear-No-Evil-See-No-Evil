#include "VisualMeter.h"
#include "Player.h"
#include "../WorldGrid/WorldGrid.h"

VisualMeter::VisualMeter(Mesh *mesh,
  vec3 position, float rotation, vec3 scale,
  vec3 direction, float velocity, vec3 dimensions,
  int scanRadius, int material = 0) :
  GameObject(mesh, position, rotation, scale,
  direction, velocity, dimensions, scanRadius, material, false) {

}

void VisualMeter::move(float time) {
}

bool VisualMeter::collide(GameObject* object) {
  /* DON"T DO NUTHIN */
  return false;
}