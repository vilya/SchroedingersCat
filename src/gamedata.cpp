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
    struct {
      int numAtoms;
      double emitFrequency;
      double maxSpeed;
      double minSpeed;
    } levelParams[] = {
      { 8, 1000.0, 0.002, 0.001 },
      { 12, 900.0, 0.003, 0.001 },
      { 16, 800.0, 0.004, 0.001 },
      { 24, 800.0, 0.004, 0.001 },
      { 32, 800.0, 0.004, 0.001 },
      { 40, 700.0, 0.004, 0.001 },
      { 50, 700.0, 0.004, 0.001 },
      { 0, 0, 0, 0 }
    };

    for (int i = 0; levelParams[i].numAtoms != 0; ++i) {
      levels.push_back(Level());
      levels.back().randomise(levelParams[i].numAtoms, levelParams[i].emitFrequency,
                              levelParams[i].maxSpeed, levelParams[i].minSpeed);
    }
  }


  //
  // Functions
  //

  void InitGameData()
  {
    assert(gGameData == NULL);
    srand48(0xCA7CA7);
    gGameData = new GameData();
  }

} // namespace cat

