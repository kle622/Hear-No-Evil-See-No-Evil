#ifndef HUD_H_
#define HUD_H_

#include "../GameObject/GameObject.h"
#include "../GameObject/Shape.h"
#include "../Library/InitObjects.h"

#include <cmath>
#include <vector>
#include <memory>

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

using namespace std;

class Hud {
	public:
		GLuint staminaBuf;
		shared_ptr<Shape> stamina;

		GLuint detectionBuf;
		shared_ptr<Shape> detection;

		GLuint dialogueBuf;
		shared_ptr<Shape> dialogue;

		GLuint norBuf;
    GLuint posBufObjG;
    GLuint norBufObjG;

		void updateStaminaHud(float stamina);
		void updateDetectionHud(float detection);
		void updateDialogueHud(int texture);
};

#endif