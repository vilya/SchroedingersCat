#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>

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

    DrawPlayArea(gGameData);
    DrawBullets(gGameData);
    DrawPlayer(gGameData);
    DrawEffects(gGameData);

    glutSwapBuffers();
  }


  void Resize(int width, int height)
  {
    if (gGameData) {
      gGameData->window.width = width;
      gGameData->window.height = height;
    }
    glViewport(0, 0, width, height);
    glutPostRedisplay();
  }


  void KeyPressed(unsigned char key, int x, int y)
  {
    const unsigned char kEsc = 27;
    if (key == kEsc)
      exit(0);

    fprintf(stderr, "key %d pressed\n", key);
  }


  void KeyReleased(unsigned char key, int x, int y)
  {
    fprintf(stderr, "key %d released\n", key);
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
        fprintf(stderr, "special key %d pressed\n", key);
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
        fprintf(stderr, "special key %d released\n", key);
        break;
    }
  }


  void MainLoop()
  {
    double frameStartTime = Now();

    // Calculations for the current frame.
    UpdateBullets(gGameData);
    UpdatePlayer(gGameData);

    double frameEndTime = Now();
    double frameTime = frameEndTime - frameStartTime;
    if (frameTime < kMinFrameTime)
      SleepFor(kMinFrameTime - frameTime);

    frameEndTime = Now();
    gGameData->gameTime += (frameEndTime - frameStartTime);

    glutPostRedisplay();
  }


  void UpdateBullets(GameData* game)
  {
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
      bullets.position[bullets.count] = Vec2(drand48(), drand48());

      // Random velocity with length between 0.005 and 0.01.
      bullets.velocity[bullets.count] = Unit(Vec2(drand48() - 0.5, drand48() - 0.5)) * 0.005;

      bullets.launchTime[bullets.count] = bullets.lastEmit;
      bullets.flags[bullets.count] = 0;

      ++bullets.count;
      fprintf(stderr, "%u bullets\n", bullets.count);
    }
  }


  void UpdatePlayer(GameData* game)
  {
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

    // TODO: handle other player inputs.
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

  cat::InitGameData();
  cat::Start();
}

