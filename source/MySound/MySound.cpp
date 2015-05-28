#include "MySound.h"
#define DEBUG

MySound* myReceiver = new MySound();

MySound::MySound() {

  this->engine = createIrrKlangDevice();
  if (!engine) {
    std::cerr << "Oh dear the sound engine did not start up" << std::endl; // error starting up the engine
  }
  //engine->play2D("../dependencies/irrKlang/media/Sinner Monologues.wav", false, false, true);
  // Play something on loop for background music
  //this->openBckGrnd = engine->play2D("../dependencies/irrKlang/media/ope.wav", false, true, true);
  //this->openBckGrnd->setVolume(.05);
  //this->openBckGrnd->setSoundStopEventReceiver(myReceiver, 0);
  //this->openBckGrnd->setIsPaused(false);
  this->backGroundSnd = engine->play2D("../dependencies/irrKlang/media/red_sky_at_night.wav", true, true, true);
  this->backGroundSnd->setVolume(.05);
  this->backGroundSnd->setIsPaused(false);

  this->noseSnd = engine->play2D("../dependencies/irrKlang/media/ow_my_nose.wav", false, true, true);
  this->footSndPlayr = engine->play2D("../dependencies/irrKlang/media/footstepsWalk2.wav", false, true, true);

  this->guardTalk = engine->play3D("../dependencies/irrKlang/media/killing_to_me.wav", vec3df(0, 0, 0), false, true, true);
  this->guardTalk->setMinDistance(1.0f);

  this->loseSnd = engine->play2D("../dependencies/irrKlang/media/its_curtains.wav", false, true, true);
  this->loseSnd->setVolume(0.5);
  this->winSnd = engine->play2D("../dependencies/irrKlang/media/victory_music.wav", false, true, true);
  this->winSnd->setVolume(0.4);
  //engine->play2D("../dependencies/irrKlang/media/killing_to_me.wav", false, false, true);
}

ISound* MySound::startSound(ISound* sound, const char* sndLocation) {
  if (sound->isFinished()) {
    printf("here so Sound is finished\n");
    sound = engine->play2D(sndLocation, false, false, true);
  }
  else if (sound->getIsPaused()) {
    printf("unpause our sound\n");
    sound->setIsPaused(false);
  }
  return sound;
}

ISound* MySound::startSound3D(ISound* sound, const char* sndLocation, vec3 position) {
  if (sound->isFinished()) {
    sound = engine->play3D(sndLocation, vec3df(position.x, position.y, position.z), false, false, true);
  }
  else if (sound->getIsPaused()) {
    sound = engine->play3D("../dependencies/irrKlang/media/killing_to_me.wav", vec3df(position.x, position.y, position.z), false, true, true);
    sound->setIsPaused(false);
  }
  return sound;
}

void MySound::setListenerPos(vec3 position, vec3 direction) {
  engine->setListenerPosition(vec3df(position.x, position.y, position.z),
    vec3df(direction.x, direction.y, direction.z));
}

void MySound::playSndExit(ISound* sound) {
  if (sound->getIsPaused()) {
    backGroundSnd->setIsPaused(true);
    sound->setIsPaused(false);
  }
  else if (sound->isFinished()) {
#ifndef DEBUG
    exit(0);
#endif
  }
}

void MySound::cleanUpSound() {
  this->backGroundSnd->drop();
  this->noseSnd->drop();
  this->footSndPlayr->drop();
  this->guardTalk->drop();
  this->engine->drop();
}

void MySound::OnSoundStopped(ISound* sound, E_STOP_EVENT_CAUSE reason, void* userData) {
  this->backGroundSnd->setIsPaused(false);
}





