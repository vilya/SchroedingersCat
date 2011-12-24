#include "gamedata.h"

#include <cassert>
#include <cstdlib>

namespace cat {
  
  //
  // Constants
  //

  const float kAtomSize = 0.03;


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
    window(),
    draw(NULL),
    levels(),
    currentLevel(NULL)
  {
    for (int i = 0; i < 2; ++i) {
      levels.push_back(Level());
      RandomLevel(levels.back());
    }
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

