#ifndef DETAILPROP_H_
#define DETAILPROP_H_

#include "GameObject.h"
#include "Mesh.h"
#include "Handles.h"

using namespace glm;

class DetailProp : public GameObject {
protected:

public:
	//Properties

	//Constructor
	DetailProp(Mesh *mesh,
		vec3 position, vec3 scale, float rotation,
		vec3 direction, float velocity, vec3 dimensions,
		int material);

	//Object methods
	bool collide(GameObject* object);
};

#endif