#include "Hud.h"

Hud::Hud() {
	initVertexObjects(&dialogueBuf, &norBuf, detectionBarVertices, normals, 6);

	this->dialogue = shared_ptr<Shape>(
		new Shape(
		      vec3(0), //position
		      0, //rotation
		      vec3(1, 1, 1), //scale
		      vec3(1, 0, 0), //direction
		      0, //velocity
		      6, //indices
		      posBufObjG, 
		      norBufObjG,
		      1 //material
		      );
		}
	);
}