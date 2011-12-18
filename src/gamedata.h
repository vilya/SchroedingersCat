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

  struct DrawingData; // Opaque structure used as a cache for graphics data; see drawing.cpp for details.


  //
  // Constants
  //

  // Maximum number of bullets that can be live at any one time.
  static const unsigned int kMaxBullets = 1024;


  //
  // Global variables
  //

  extern GameData* gGameData;


  //
  // Types
  //

  struct PlayerData {
    Vec2 position;
    Vec2 size;
    int livesRemaining;

    PlayerData();
  };


  struct BulletData {
    unsigned int count;
    double bulletSize;
    double lastEmit;
    double halfLife;

    Vec2 position[kMaxBullets];
    Vec2 velocity[kMaxBullets];
    double launchTime[kMaxBullets];
    unsigned int flags[kMaxBullets];

    BulletData();
  };


  struct WindowData {
    int width;
    int height;

    bool keyPressed[256];
    bool leftPressed;
    bool rightPressed;
    bool upPressed;
    bool downPressed;

    WindowData();
  };


  struct GameData {
    // The current elapsed time for the game. We can't just use the system
    // time, because we need to be able to pause it.
    double gameTime;
    // Player state.
    PlayerData player;
    // Bullet state.
    BulletData particles;
    // Data about the game window.
    WindowData window;
    // Cached drawing data.
    DrawingData* draw;

    GameData();
  };


  //
  // Functions
  //

  // Creates and initialises the global game data instance. You must call this
  // before you use the gGameData pointer. You should only call it once.
  void InitGameData();

} // namespace cat

#endif // cat_gamedata_h

