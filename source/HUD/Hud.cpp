#include "Hud.h"

GLfloat detectionBarVertices[] = {
    -1.0f, 0.0f, -1.0f,
    -1.0f, 0.0f, -0.5f,
    1.0f, 0.0f, -1.0f,

    -1.0f, 0.0f, -0.5f,
    1.0f, 0.0f, -1.0f,
    1.0f, 0.0f, -0.5f,
};

GLfloat normals[] = {
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
};

Hud::Hud() {

  initVertexObject(&this->dialogueBuf, &this->norBuf, &this->idxBufObjG, &this->texBufObjG, detectionBarVertices, normals, 6);

	this->detection = shared_ptr<Shape>(
		new Shape(
		      vec3(0), //position
		      0, //rotation
		      vec3(1, 1, 1), //scale
		      vec3(1, 0, 0), //direction
		      0, //velocity
		      6, //indices
		      this->posBufObjG, 
		      this->norBufObjG,
          this->idxBufObjG,
          this->texBufObjG,
		      1 //material
		      ));
  //this->dialogue->loadMipmapTexture(resPath(sysPath("textures", "concrete.bmp")));
}

void Hud::updateStaminaHud(float stamina){

}

void Hud::updateDetectionHud(float detection) {

}

void Hud::updateDialogueHud(int texture) {

	
}
