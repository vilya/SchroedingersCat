#include "gamedata.h"

#include <cassert>

namespace cat {
  
  //
  // Global variables
  //

  GameData* gGameData = NULL;


  //
  // PlayerData public methods
  //

  PlayerData::PlayerData() :
    position(0.5, 0.5),
    size(0.02, 0.02),
    livesRemaining(9)
  {
  }


  //
  // BulletData public methods
  //

  BulletData::BulletData() :
    count(0),
    bulletSize(0.01),
    lastEmit(0),
    halfLife(1000.0)
  {
  }


  //
  // WindowData public methods
  //

  WindowData::WindowData() :
    width(0),
    height(0)
  {
  }


  //
  // GameData public methods
  //

  GameData::GameData() :
    gameState(eGameOver),
    gameTime(0),
    player(),
    particles(),
    window(),
    draw(NULL)
  {
  }


  //
  // Functions
  //

  void InitGameData()
  {
    assert(gGameData == NULL);
    gGameData = new GameData();
  }

} // namespace cat

