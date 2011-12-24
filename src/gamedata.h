#ifndef cat_gamedata_h
#define cat_gamedata_h

#include "level.h"
#include "vec2.h"

#include <list>

namespace cat {

  //
  // Forward declarations
  //

  struct GameData;
  struct PlayerData;

  struct DrawingData; // Opaque structure used as a cache for graphics data; see drawing.cpp for details.


  //
  // Constants
  //

  extern const float kAtomSize;


  //
  // Global variables
  //

  extern GameData* gGameData;


  //
  // Types
  //

  enum GameState {
    eGameTitleScreen,
    eGameStartingLevel,
    eGamePlaying,
    eGameFinishedLevel,
    eGameOver,
    eGamePaused
  };


  enum PowerUp {
    ePowerUpNone,
    ePowerUpSuperposition,  // Temporary invulnerability
    ePowerUpEntangling,     // When the entanglement power up is launching.
    ePowerUpEntanglement,   // Create a copy of yourself which matches your movement.

    ePowerUpCount           // Sentinel value.
  };


  enum PlayerView {
    ePlayerFront,
    ePlayerBack
  };


  struct PlayerData {
    Vec2 position;
    Vec2 size;
    int livesRemaining;
    PowerUp powerUp;
    double powerUpExpireTime;
    PlayerView view;
    int superpositionsRemaining;
    int entanglementsRemaining;
    bool collision;

    PlayerData();
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
    // Current state of the game (playing, game over, etc).
    GameState gameState;
    // The current elapsed time for the game. We can't just use the system
    // time, because we need to be able to pause it.
    double gameTime;
    // The time at which the game state changed to its current value.
    double stateChangeTime;
    // Player state.
    PlayerData player;
    // Data about the game window.
    WindowData window;
    // Cached drawing data.
    DrawingData* draw;
    // Levels.
    std::list<Level> levels;
    std::list<Level>::iterator currentLevel;

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

