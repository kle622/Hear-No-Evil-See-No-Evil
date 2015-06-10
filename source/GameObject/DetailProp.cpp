#include "DetailProp.h"

DetailProp::DetailProp(Mesh *mesh,
	vec3 position, vec3 scale, float rotation,
	vec3 direction, float velocity, vec3 dimensions, int material) :
	GameObject(mesh, position, scale, rotation,
	direction, velocity, dimensions, 0, material, GameObject::ObjectType::DETAIL) {}

bool DetailProp::collide(GameObject* object) {
	return false;
}