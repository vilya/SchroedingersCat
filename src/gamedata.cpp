#include "gamedata.h"

#include <cassert>
#include <cstdlib>

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
    livesRemaining(9),
    powerUp(ePowerUpNone),
    powerUpExpireTime(0),
    view(ePlayerFront),
    superpositionsRemaining(0),
    entanglementsRemaining(0),
    collision(false)
  {
  }


  //
  // BulletData public methods
  //

  BulletData::BulletData() :
    count(0),
    bulletSize(0.03),
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
    gameState(eGameTitleScreen),
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

