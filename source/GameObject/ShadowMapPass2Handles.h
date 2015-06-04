#ifndef __PASS2__HANDLES__
#define __PASS2__HANDLES__
#define MAX_LIGHTS 10
#include "GameObject.h"
#include "Shape.h"
#include "Player.h"

class Pass2Handles  : public Handles {
 public:
  GLuint shadowMap, uDepthMVP, hasTex, texture, aTexCoord, uNumLights, detectionLevel, uAllLightsPosition[MAX_LIGHTS], 
    uAllLightsAttenuation[MAX_LIGHTS], uAllLightsConeDirection[MAX_LIGHTS],
    uAllLightsConeAngle[MAX_LIGHTS], uConeDirection, uConeAngle, uAttenuation, uAllLights[MAX_LIGHTS], isGlass;
  bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
  void draw(GameObject* obj);
  void draw(Shape* obj);
};


#endif
