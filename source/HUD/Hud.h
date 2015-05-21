#ifndef HUD_H_
#define HUD_H_

#include "../GameObject/GameObject.h"
#include "../GameObject/Shape.h"

#include <cmath>
#include <vector>
#include <memory>

using namespace std;

class Hud {
	public:
		shared_ptr<Shape> stamina;
		shared_ptr<Shape> detection;
		shared_ptr<Shape> dialogue;
		void updateStaminaHud(float stamina);
		void updateDetectionHud(float detection);
		void updateDialogueHud(int texture);
};

#endif