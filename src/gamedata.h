#ifndef cat_gamedata_h
#define cat_gamedata_h

#include "vec2.h"

namespace cat {

  //
  // Forward declarations
  //

  struct GameData;
  struct PlayerData;
  struct BulletData;


  //
  // Constants
  //

  // Maximum number of bullets that can be live at any one time.
  static const int kMaxBullets = 10000;


  //
  // Global variables
  //

  extern GameData* gGameData;


  //
  // Types
  //

  struct PlayerData {
    Vec2 position;
    int livesRemaining;

    PlayerData();
  };


  struct BulletData {
    unsigned int count;
    Vec2 position[kMaxBullets];
    Vec2 velocity[kMaxBullets];
    double launchTime[kMaxBullets];
    unsigned int flags[kMaxBullets];

    BulletData();
  };


  struct GameData {
    // The current elapsed time for the game. We can't just use the system
    // time, because we need to be able to pause it.
    double t;
    // Player state.
    PlayerData player;
    // Bullet state.
    BulletData particles;

    GameData();
  };


  //
  // Functions
  //

  // Creates and initialises the global game data instance. You must call this
  // before you use the gGameData pointer. You should only call it once.
  void initGameData();

} // namespace cat

#endif // cat_gamedata_h

