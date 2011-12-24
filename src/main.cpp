#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <libgen.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#ifdef linux
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#endif

#include "drawing.h"
#include "gamedata.h"

namespace cat {

  //
  // Constants
  //

  const char* kGameName = "Schroedinger's Cat: The Game";
  const char* kCopyrightMessage = "(c) Vilya Harvey, 2011";

  static const int kWindowWidth = 800;
  static const int kWindowHeight = 800;

  static const double kMinFrameTime = 1000.0 / 60.0; // Targetting 60 fps.


  //
  // Forward declarations
  //

  void Start();
  void Render();
  void Resize(int x, int y);
  void KeyPressed(unsigned char key, int x, int y);
  void KeyReleased(unsigned char key, int x, int y);
  void SpecialKeyPressed(int key, int x, int y);
  void SpecialKeyReleased(int key, int x, int y);
  void MainLoop();

  void UpdateAtoms(GameData* game);
  void UpdatePlayer(GameData* game);
  void UpdateGameState(GameData* game);

  void StartNewGame(GameData* game);
  void StartNewLife(GameData* game);

  void SetGameState(GameData* game, GameState state);
  void SetPowerUp(GameData* game, PowerUp powerUp);

  // Get the current system time in milliseconds (may include a fraction of a millisecond).
  double Now();
  void SleepFor(double milliseconds);


  //
  // Functions
  //

  void Start()
  {
    int argc = 0;
    char* argv[] = { NULL };

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(kWindowWidth, kWindowHeight);
    glutCreateWindow(kGameName);

    glutDisplayFunc(Render);
    glutReshapeFunc(Resize);
    glutKeyboardFunc(KeyPressed);
    glutKeyboardUpFunc(KeyReleased);
    glutSpecialFunc(SpecialKeyPressed);
    glutSpecialUpFunc(SpecialKeyReleased);
    glutIdleFunc(MainLoop);

    InitDrawing(gGameData);

    glutMainLoop(); // This doesn't return until the main window closes.
  }


  void Render()
  {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, 0, 4);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (gGameData->gameState) {
    case eGameTitleScreen:
      DrawPlayArea(gGameData);
      DrawTitles(gGameData);
      break;
    case eGameStartingLevel:
      DrawPlayArea(gGameData);
      DrawPlayer(gGameData);
      DrawLevelCountdown(gGameData);
      break;
    case eGamePlaying:
      DrawPlayArea(gGameData);
      DrawAtoms(gGameData);
      DrawPlayer(gGameData);
      DrawHUD(gGameData);
      break;
    case eGameFinishedLevel:
      DrawPlayArea(gGameData);
      DrawPlayer(gGameData);
      DrawLevelComplete(gGameData);
      break;
    case eGameOver:
      DrawPlayArea(gGameData);
      DrawGameOver(gGameData);
      break;
    case eGameVictory:
      DrawPlayArea(gGameData);
      DrawVictory(gGameData);
      break;
    case eGamePaused:
      DrawPlayArea(gGameData);
      DrawAtoms(gGameData);
      DrawPlayer(gGameData);
      DrawHUD(gGameData);
      DrawPause(gGameData);
      break;
    }

    glutSwapBuffers();
  }


  void Resize(int width, int height)
  {
    glViewport(0, 0, width, height);
    if (gGameData) {
      gGameData->window.width = width;
      gGameData->window.height = height;
      WindowResized(gGameData);
    }
    glutPostRedisplay();
  }


  void KeyPressed(unsigned char key, int x, int y)
  {
    const unsigned char kEsc = 27;
    const unsigned char kSpace = 32;

    switch (key) {
      case kEsc:
        if (gGameData->gameState == eGamePlaying || gGameData->gameState == eGamePaused)
          SetGameState(gGameData, eGameOver);
        else
          exit(0);
        break;

      case kSpace:
        if (gGameData->gameState == eGamePlaying)
          SetGameState(gGameData, eGamePaused);
        else if (gGameData->gameState == eGamePaused)
          SetGameState(gGameData, eGamePlaying);
        else
          gGameData->window.keyPressed[key] = true;
        break;

      default:
        gGameData->window.keyPressed[key] = true;
        break;
    }
  }


  void KeyReleased(unsigned char key, int x, int y)
  {
    gGameData->window.keyPressed[key] = false;
  }


  void SpecialKeyPressed(int key, int x, int y)
  {
    switch (key) {
      case GLUT_KEY_LEFT:
        gGameData->window.leftPressed = true;
        break;
      case GLUT_KEY_RIGHT:
        gGameData->window.rightPressed = true;
        break;
      case GLUT_KEY_UP:
        gGameData->window.upPressed = true;
        break;
      case GLUT_KEY_DOWN:
        gGameData->window.downPressed = true;
        break;
      default:
        break;
    }
  }


  void SpecialKeyReleased(int key, int x, int y)
  {
    switch (key) {
      case GLUT_KEY_LEFT:
        gGameData->window.leftPressed = false;
        break;
      case GLUT_KEY_RIGHT:
        gGameData->window.rightPressed = false;
        break;
      case GLUT_KEY_UP:
        gGameData->window.upPressed = false;
        break;
      case GLUT_KEY_DOWN:
        gGameData->window.downPressed = false;
        break;
      default:
        break;
    }
  }


  void MainLoop()
  {
    double frameStartTime = Now();

    switch (gGameData->gameState) {
    case eGameStartingLevel:
      UpdatePlayer(gGameData);
      break;
    case eGamePlaying:
      // Calculations for the current frame.
      UpdateAtoms(gGameData);
      UpdatePlayer(gGameData);
      break;
    case eGameFinishedLevel:
      UpdatePlayer(gGameData);
      break;
    default:
      break;
    }
    UpdateGameState(gGameData);

    glutPostRedisplay();

    double frameEndTime = Now();
    double frameTime = frameEndTime - frameStartTime;
    if (frameTime < kMinFrameTime)
      SleepFor(kMinFrameTime - frameTime);

    if (gGameData->gameState != eGamePaused) {
      frameEndTime = Now();
      gGameData->gameTime += (frameEndTime - frameStartTime);
    }
  }


  void UpdateAtoms(GameData* game)
  {
    assert(game != NULL);

    if (game->currentLevel == game->levels.end())
      return;

    Level& level = *game->currentLevel;
    Vec2 bottomLeft(kAtomSize / 2.0, kAtomSize / 2.0);
    Vec2 topRight(1.0 - kAtomSize / 2.0, 1.0 - kAtomSize / 2.0);

    // Move existing atoms
    for (unsigned int i = 0; i < level.atomCount; ++i) {
      Vec2 pos = level.position[i];
      Vec2 vel = level.velocity[i];

      pos += vel;
      
      if (pos.x < bottomLeft.x) {
        pos.x = bottomLeft.x + (bottomLeft.x - pos.x);
        vel.x = -vel.x;
      }
      else if (pos.x > topRight.x) {
        pos.x = topRight.x - (pos.x - topRight.x);
        vel.x = -vel.x;
      }

      if (pos.y < bottomLeft.y) {
        pos.y = bottomLeft.y + (bottomLeft.y - pos.y);
        vel.y = -vel.y;
      }
      else if (pos.y > topRight.y) {
        pos.y = topRight.y - (pos.y - topRight.y);
        vel.y = -vel.y;
      }

      level.position[i] = pos;
      level.velocity[i] = vel;
    }

    // Emit new atoms
    double levelTime = game->gameTime - game->stateChangeTime;
    while (level.atomCount < kMaxAtoms && level.launchTime[level.atomCount] <= levelTime)
      ++level.atomCount;
  }


  void UpdatePlayer(GameData* game)
  {
    assert(game != NULL);

    WindowData& win = game->window;
    PlayerData& player = game->player;

    // Handle player movement.
    if (win.leftPressed || win.rightPressed || win.upPressed || win.downPressed) {
      const double kScale = 0.01;
      const Vec2 kRadius = player.size / 2.0;

      Vec2 velocity;
      if (win.leftPressed)
        velocity.x -= 1;
      if (win.rightPressed)
        velocity.x += 1;
      if (win.upPressed)
        velocity.y += 1;
      if (win.downPressed)
        velocity.y -= 1;
      velocity = Unit(velocity) * kScale;

      player.position += velocity;
      player.view = (velocity.y > 0) ? ePlayerBack : ePlayerFront;

      Vec2 bottomLeft = player.position - kRadius;
      Vec2 topRight = player.position + kRadius;

      if (bottomLeft.x < 0.0)
        player.position.x = kRadius.x;
      else if (topRight.x > 1.0)
        player.position.x = 1.0 - kRadius.x;

      if (bottomLeft.y < 0.0)
        player.position.y = kRadius.y;
      else if (topRight.y > 1.0)
        player.position.y = 1.0 - kRadius.y;
    }

    // Check whether a power up is expiring.
    switch (player.powerUp) {
      case ePowerUpSuperposition:
      case ePowerUpEntanglement:
        if (game->gameTime >= player.powerUpExpireTime)
          player.powerUp = ePowerUpNone;
        break;
      case ePowerUpEntangling:
        if (game->gameTime >= player.powerUpExpireTime)
          player.powerUp = ePowerUpEntanglement;
        break;
      default:
        break;
    }

    // Check whether the player is launching a power-up.
    if (player.powerUp == ePowerUpNone) {
      if (win.keyPressed['s'] && player.superpositionsRemaining > 0) {
        SetPowerUp(game, ePowerUpSuperposition);
        --player.superpositionsRemaining;
      }
      else if (win.keyPressed['d'] && player.entanglementsRemaining > 0) {
        SetPowerUp(game, ePowerUpEntangling);
        --player.entanglementsRemaining;
      }
    }
  }


  void UpdateGameState(GameData* game)
  {
    assert(game != NULL);

    double elapsed = game->gameTime - game->stateChangeTime;
    bool anyKeyPressed = false;
    for (int i = 0; i < 256; ++i) {
      if (game->window.keyPressed[i]) {
        anyKeyPressed = true;
        break;
      }
    }

    switch (game->gameState) {
    case eGameTitleScreen:
      if (anyKeyPressed)
        StartNewGame(game);
      break;

    case eGamePaused:
      if (anyKeyPressed)
        SetGameState(game, eGamePlaying);
      break;

    case eGameOver:
      if (anyKeyPressed)
        StartNewGame(game);
      else if (elapsed >= 5000.0)
        SetGameState(game, eGameTitleScreen);
      break;

    case eGameVictory:
      if (anyKeyPressed)
        StartNewGame(game);
      else if (elapsed >= 30000.0)
        SetGameState(game, eGameTitleScreen);
      break;

    case eGameStartingLevel:
      if (elapsed >= 3000.0) {
        game->currentLevel->startLevel();
        SetGameState(game, eGamePlaying);
      }
      break;

    case eGameFinishedLevel:
      if (elapsed >= 3000.0) {
        ++game->currentLevel;
        if (game->currentLevel != game->levels.end())
          SetGameState(game, eGameStartingLevel);
        else
          SetGameState(game, eGameVictory);
      }
      break;

    case eGamePlaying:
      {
        Level& level = *game->currentLevel;
        if (elapsed >= level.duration) {
          SetGameState(game, eGameFinishedLevel);
          break;
        }

        PlayerData& player = game->player;
        if (player.powerUp == ePowerUpSuperposition) {
          player.collision = false;
          break;
        }
        // TODO: add handling for entanglement.
        if (player.collision) {
          --player.livesRemaining;
          if (player.livesRemaining <= 0)
            SetGameState(game, eGameOver);
          else
            StartNewLife(game);
        }
      }
    }
  }


  void StartNewGame(GameData* game)
  {
    SetGameState(game, eGameStartingLevel);
    game->gameTime = 0;

    game->player.size = Vec2(0.06, 0.06);
    game->player.livesRemaining = 9;
    game->player.superpositionsRemaining = 3;
    game->player.entanglementsRemaining = 1;

    game->currentLevel = game->levels.begin();

    StartNewLife(game);
  }


  void StartNewLife(GameData* game)
  {
    game->player.position = Vec2(0.5, 0.5);
    game->player.collision = false;
    SetPowerUp(game, ePowerUpNone);

    game->currentLevel->startLevel();
    SetGameState(game, eGameStartingLevel);
  }


  void SetGameState(GameData* game, GameState state)
  {
    game->gameState = state;
    game->stateChangeTime = game->gameTime;
  }


  void SetPowerUp(GameData* game, PowerUp powerUp)
  {
    const double kPowerUpDuration[] = { 0.0, 2000.0, 1000.0, 60000.0 };
    double duration = kPowerUpDuration[powerUp];

    game->player.powerUp = powerUp;
    game->player.powerUpExpireTime = game->gameTime + duration;
  }


  double Now()
  {
    struct timeval t;
    gettimeofday(&t, NULL);
    
    double result = t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
    return result;
  }


  void SleepFor(double milliseconds)
  {
    usleep((useconds_t)(milliseconds * 1000));
  }

} // namespace cat

int main(int argc, char** argv)
{
  printf("%s\n", cat::kGameName);
  printf("%s\n", cat::kCopyrightMessage);

  chdir(dirname(argv[0]));

  cat::InitGameData();
  cat::Start();
}

