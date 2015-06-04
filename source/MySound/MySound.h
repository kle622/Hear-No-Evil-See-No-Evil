#ifndef MYSOUND_H_
#define MYSOUND_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <time.h>
#include <iostream>
#include <cassert>
#include "../Library/GLSL.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "../glm/gtc/type_ptr.hpp" //value_ptr
//#include "../../dependencies/irrKlang/include/conio.h"
//#include "../../dependencies/irrKlang/include/irrKlang.h"
#include "conio.h"
#include "irrKlang.h"

#if defined WIN32
#include <windows.h>
inline void sleepSomeTime() { Sleep(100); }
#endif

using namespace irrklang;
using namespace glm;

class MySound : public irrklang::ISoundStopEventReceiver {
public:
  ISoundEngine* engine;
  ISound* noseSnd, *guardTalk, *backGroundSnd, *footSndPlayr, *loseSnd, *winSnd, *openBckGrnd, *collectableSnd, *openingSnd;

  //ConstrbackGroundSnd;uctor
  MySound();

  //Sound Methods
  void cleanUpSound();
  ISound* startSound(ISound* sound, const char* sndLocation);
  ISound* startSound3D(ISound* sound, const char* sndLocation, vec3 position);
  void setListenerPos(vec3 position, vec3 direction);
  void playSndExit(ISound* sound);
  virtual void OnSoundStopped(ISound* sound, E_STOP_EVENT_CAUSE reason, void* userData);
};
#endif
