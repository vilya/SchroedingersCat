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

  void UpdateBullets(GameData* game);
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
    case eGamePlaying:
      DrawPlayArea(gGameData);
      DrawBullets(gGameData);
      DrawPlayer(gGameData);
      DrawEffects(gGameData);
      DrawHUD(gGameData);
      break;
    case eGameOver:
      DrawPlayArea(gGameData);
      DrawGameOver(gGameData);
      break;
    case eGamePaused:
      DrawPlayArea(gGameData);
      DrawBullets(gGameData);
      DrawPlayer(gGameData);
      DrawEffects(gGameData);
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
    case eGamePlaying:
      // Calculations for the current frame.
      UpdateBullets(gGameData);
      UpdatePlayer(gGameData);
      UpdateGameState(gGameData);
      break;
    default:
      UpdateGameState(gGameData);
      break;
    }

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


  void UpdateBullets(GameData* game)
  {
    assert(game != NULL);

    BulletData& bullets = game->particles;

    Vec2 bottomLeft(bullets.bulletSize, bullets.bulletSize);
    Vec2 topRight(1.0 - bullets.bulletSize, 1.0 - bullets.bulletSize);

    // Expire bullets?

    // Move bullets
    for (unsigned int i = 0; i < bullets.count; ++i) {
      Vec2 pos = bullets.position[i];
      Vec2 vel = bullets.velocity[i];

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

      bullets.position[i] = pos;
      bullets.velocity[i] = vel;
    }

    // Emit new bullets
    while (bullets.count < kMaxBullets && game->gameTime >= bullets.lastEmit + bullets.halfLife) {
      bullets.lastEmit += bullets.halfLife;
      bullets.halfLife = pow(bullets.halfLife, 0.999);

      // Random emit position anywhere in the play area.
      int wall = int(drand48() * 4) % 4;
      float angle = (drand48() * 0.9 + 0.05) * M_PI; // in radians, 0 is parallel to +ve x axis, pi/2 is +ve y axis
      float speed = drand48() * 0.003 + 0.001; // Random speed between 0.001 and 0.004
      switch (wall) {
      case 0: // left wall
        bullets.position[bullets.count] = Vec2(0, drand48());
        angle += M_PI_2;
        break;
      case 1: // top wall
        bullets.position[bullets.count] = Vec2(drand48(), 1);
        angle += M_PI;
        break;
      case 2: // right wall
        bullets.position[bullets.count] = Vec2(1, drand48());
        angle -= M_PI_2;
        break;
      case 3: // bottom wall
        bullets.position[bullets.count] = Vec2(drand48(), 0);
        break;
      }

      // Random velocity with length between 0.005 and 0.01.
      bullets.velocity[bullets.count] = Vec2(cos(angle), sin(angle)) * speed;
      //bullets.velocity[bullets.count] = Vec2(0, 0);

      bullets.launchTime[bullets.count] = bullets.lastEmit;
      bullets.flags[bullets.count] = 0;

      ++bullets.count;
    }
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

    if (game->gameState == eGameOver || game->gameState == eGameTitleScreen) {
      for (int i = 0; i < 256; ++i) {
        if (game->window.keyPressed[i]) {
          StartNewGame(game);
          return;
        }
      }
    }
    else if (game->gameState == eGamePaused) {
      for (int i = 0; i < 256; ++i) {
        if (game->window.keyPressed[i]) {
          SetGameState(game, eGamePlaying);
          return;
        }
      }
    }

    if (game->gameState == eGameOver) {
      float elapsed = game->gameTime - game->stateChangeTime;
      if (elapsed >= 5000)
        SetGameState(game, eGameTitleScreen);
      return;
    }

    if (game->gameState == eGameTitleScreen)
      return;

    // Check for collisions between the player and the bullets.
    PlayerData& player = game->player;
    BulletData& bullets = game->particles;

    if (player.powerUp == ePowerUpSuperposition) {
      // Not affected by collisions when superposed.
      player.collision = false;
      return;
    }

    // TODO: add handling for the entanglement power up.

    if (player.collision) {
      --player.livesRemaining;
      if (player.livesRemaining <= 0)
        SetGameState(game, eGameOver);
      else
        StartNewLife(game);
    }
  }


  void StartNewGame(GameData* game)
  {
    SetGameState(game, eGamePlaying);
    game->gameState = eGamePlaying;
    game->gameTime = 0;

    game->player.size = Vec2(0.06, 0.06);
    game->player.livesRemaining = 9;
    game->player.superpositionsRemaining = 3;
    game->player.entanglementsRemaining = 1;
    StartNewLife(game);

    game->particles.count = 0;
    game->particles.bulletSize = 0.04;
    game->particles.lastEmit = 0;
    game->particles.halfLife = 1000.0;
  }


  void StartNewLife(GameData* game)
  {
    game->player.position = Vec2(0.5, 0.5);
    game->player.collision = false;
    SetPowerUp(game, ePowerUpSuperposition);
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

