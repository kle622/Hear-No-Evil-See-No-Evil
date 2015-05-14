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

#if defined WIN32
#include <windows.h>
#include <conio.h>
inline void sleepSomeTime() { Sleep(100); }
#pragma comment(lib, "../dependencies/irrKlang/lib/Winx64-visualStudio/irrKlang.lib") // link with irrKlang.dll
#else
#include "../dependencies/irrKlang/examples/common/conio.h"
//#pragma comment(lib, "../dependencies/irrKlang/lib/irrKlang.lib") // link with irrKlang.dll
#endif
//#if defined __APPLE__ && __MACH__
//#include "..dependencies/irrKlang/examples/common/conio.h"
//#endif
#include "../../dependencies/irrKlang/include/irrKlang.h"

using namespace irrklang;
using namespace glm;

class MySound : public irrklang::ISoundStopEventReceiver {
public:
  ISoundEngine* engine;
  ISound* noseSnd, *guardTalk, *backGroundSnd, *footSndPlayr, *loseSnd, *winSnd, *openBckGrnd;

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
