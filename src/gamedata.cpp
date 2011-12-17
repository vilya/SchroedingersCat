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
    position(),
    livesRemaining(9)
  {
  }


  //
  // BulletData public methods
  //

  BulletData::BulletData() :
    count(0)
  {
  }


  //
  // GameData public methods
  //

  GameData::GameData() :
    t(0),
    player(),
    particles()
  {
  }


  //
  // Functions
  //

  void initGameData()
  {
    assert(gGameData == NULL);
    gGameData = new GameData();
  }

} // namespace cat

