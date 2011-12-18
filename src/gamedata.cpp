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
