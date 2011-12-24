#include "drawing.h"

#include "gamedata.h"
#include "image.h"
#include "level.h"
#include "resource.h"

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#ifdef linux
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#endif

namespace cat {

  //
  // Constants
  //

  static const float kFloorZ = -1;
  static const float kPlayerZ = -0.5;
  static const float kAtomZ = -0.2;
  static const float kTextZ = -0.1;

  static const float kCharHeight = 21;


  //
  // Types
  //

  // Horizontal string alignment.
  enum StringAlignment {
    eAlignLeft,
    eAlignRight,
    eAlignCenter
  };


  struct DrawingData {
    GLuint floorTextureID;
    GLuint playerFrontTextureID[ePowerUpCount];
    GLuint playerBackTextureID[ePowerUpCount];
    GLuint particleTextureID;
    GLuint titleTextureID;

    GLuint collisionQueryID;
    GLuint maxPixelsDrawn;

    DrawingData();
    ~DrawingData();
  };


  //
  // Forward declarations
  //

  GLuint UploadTexture(const char* filename);
  void DrawQuad(double x, double y, double z, double w, double h, GLuint textureID);
  void DrawText(double x, double y, const char* text, StringAlignment alignment);
  float StringWidth(void* font, const char* text);
  bool CheckGLError(const char *errMsg);


  //
  // DrawingData public methods
  //

  DrawingData::DrawingData() :
    floorTextureID(0),
    particleTextureID(0),
    titleTextureID(0),
    collisionQueryID(0),
    maxPixelsDrawn(0)
  {
    // Load the floor texture.
    floorTextureID = UploadTexture(ResourcePath("floor_alt.jpg"));

    // Load the player textures.
    const char* frontTexturePaths[] = {
      "player_front_nopowerup.tif",
      "player_front_superposition.tif",
      "player_front_entangling.tif",
      "player_front_entanglement.tif"
    };
    const char* backTexturePaths[] = {
      "player_back_nopowerup.tif",
      "player_back_superposition.tif",
      "player_back_entangling.tif",
      "player_back_entanglement.tif"
    };
    for (int p = ePowerUpNone; p < ePowerUpCount; ++p) {
      playerFrontTextureID[p] = UploadTexture(ResourcePath(frontTexturePaths[p]));
      playerBackTextureID[p] = UploadTexture(ResourcePath(backTexturePaths[p]));
    }

    // Load the particle textures.
    particleTextureID = UploadTexture(ResourcePath("atom.tif"));

    // Load the title screen texture.
    titleTextureID = UploadTexture(ResourcePath("TitleScreen.tif"));

    // Create a query object which we'll use for collision detection.
    glGenQueries(1, &collisionQueryID);
  }


  DrawingData::~DrawingData()
  {
    if (floorTextureID)
      glDeleteTextures(1, &floorTextureID);
    for (int p = ePowerUpNone; p < ePowerUpCount; ++p) {
      if (playerFrontTextureID[p])
        glDeleteTextures(1, &playerFrontTextureID[p]);
      if (playerBackTextureID[p])
        glDeleteTextures(1, &playerBackTextureID[p]);
    }
    if (particleTextureID)
      glDeleteTextures(1, &particleTextureID);
    if (collisionQueryID)
      glDeleteQueries(1, &collisionQueryID);
  }


  //
  // Public functions
  //

  void InitDrawing(GameData* game)
  {
    glEnable(GL_DEPTH_TEST);
    assert(game->draw == NULL);
    game->draw = new DrawingData();
  }


  void DrawPlayArea(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    DrawQuad(0, 0, kFloorZ, 1, 1, game->draw->floorTextureID);
  }


  void DrawPlayer(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    PlayerData& player = game->player;
    DrawingData* draw = game->draw;

    Vec2 bottomLeft = player.position - player.size / 2.0;

    GLuint textureID;
    switch (player.view) {
      case ePlayerFront:
        textureID = draw->playerFrontTextureID[player.powerUp];
        break;
      case ePlayerBack:
        textureID = draw->playerBackTextureID[player.powerUp];
        break;
    }

    // Keep track of how many pixels are drawn for the player. If this is less
    // than normal it means the player's hit something.
    glBeginQuery(GL_SAMPLES_PASSED, draw->collisionQueryID);
    DrawQuad(bottomLeft.x, bottomLeft.y, kPlayerZ, player.size.x, player.size.y, textureID);
    glEndQuery(GL_SAMPLES_PASSED);

    GLuint pixelsDrawn = 0;
    glGetQueryObjectuiv(draw->collisionQueryID, GL_QUERY_RESULT, &pixelsDrawn);

    // On first frame, the player will be unobscured so the value should only
    // ever go down when a particle obscures part of the player.
    if (pixelsDrawn > draw->maxPixelsDrawn)
      draw->maxPixelsDrawn = pixelsDrawn; 
    else if (pixelsDrawn < draw->maxPixelsDrawn)
      player.collision = true;
  }


  void DrawAtoms(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    if (game->currentLevel == game->levels.end())
      return;

    Level& level = *game->currentLevel;
    WindowData& win = game->window;

    GLfloat atomSize = std::min(win.width, win.height) * kAtomSize;
    if (atomSize < 1)
      atomSize = 1;
    glPointSize(atomSize);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, game->draw->particleTextureID);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

    glBegin(GL_POINTS);
    for (unsigned int i = 0; i < level.atomCount; ++i)
      glVertex3d(level.position[i].x, level.position[i].y, kAtomZ);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_POINT_SPRITE);
    glDisable(GL_BLEND);
  }


  void DrawHUD(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    if (game->currentLevel == game->levels.end())
      return;

    Level& level = *game->currentLevel;
    WindowData& win = game->window;

    float top = win.height - kCharHeight - 10;
    float bottom = 10 + kCharHeight;
    char msg[1024];
    double timeElapsed = game->gameTime - game->stateChangeTime;
    double timeLeft = level.duration - timeElapsed;

    snprintf(msg, 1024, "Remaining %1.2lfs", timeLeft / 1000.0);
    DrawText(10, top, msg, eAlignLeft);

    snprintf(msg, 1024, "%d lives", game->player.livesRemaining);
    DrawText(10, top, msg, eAlignRight);

    snprintf(msg, 1024, "Superposition: %d\nEntanglement: %d",
             game->player.superpositionsRemaining,
             game->player.entanglementsRemaining);
    DrawText(10, bottom, msg, eAlignLeft);
  }


  void DrawTitles(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    DrawQuad(0.1, 0.5, kTextZ, 0.8, 0.3, game->draw->titleTextureID);
    glDisable(GL_BLEND);

    float y = game->window.height / 3.0;

    //DrawText(0, y, "(c) Vilya Harvey, 2011", eAlignCenter);
    //y -= kCharHeight * 2;
    DrawText(0, y, "Dedicated to William Harvey and his family", eAlignCenter);
    y -= kCharHeight * 2;
    DrawText(0, y, "Press [space] to start, [esc] to quit", eAlignCenter);
  }


  void DrawGameOver(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    float y = (game->window.height - kCharHeight) / 2.0;
    DrawText(0, y, "GAME OVER\nPress [space] to try again, [esc] to quit", eAlignCenter);
  }


  void DrawPause(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    float y = (game->window.height - kCharHeight) / 2.0;
    DrawText(0, y, "PAUSED\nPress [space] to continue, [esc] to quit", eAlignCenter);
  }


  void DrawLevelComplete(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    float y = (game->window.height - kCharHeight) / 2.0;
    DrawText(0, y, "Level complete!", eAlignCenter);
  }


  void DrawLevelCountdown(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    if (game->currentLevel == game->levels.end())
      return;

    Level& level = *game->currentLevel;
    float timeLeft = (game->stateChangeTime + 3000.0 - game->gameTime) / 1000.0;
    char timeLeftStr[256];
    snprintf(timeLeftStr, 256, "%d...", int(ceil(timeLeft)));

    float y = (game->window.height - kCharHeight * 2) * 2.0 / 3.0;

    DrawText(0, y, level.name.c_str(), eAlignCenter);
    y += kCharHeight;
    DrawText(0, y, timeLeftStr, eAlignCenter);
  }


  void WindowResized(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);
    game->draw->maxPixelsDrawn = 0;
  }


  //
  // Internal functions
  //

  GLuint UploadTexture(const char* filename)
  {
    Image* img = new Image(filename);
    assert(img);

    img->uploadTexture();
    GLuint texID = img->getTexID();

    delete img;
    return texID;
  }


  void DrawQuad(double x, double y, double z, double w, double h, GLuint textureID)
  {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
      glTexCoord2f(0, 0);
      glVertex3d(x, y, z);

      glTexCoord2f(1, 0);
      glVertex3d(x + w, y, z);

      glTexCoord2f(1, 1);
      glVertex3d(x + w, y + h, z);

      glTexCoord2f(0, 1);
      glVertex3d(x, y + h, z);
    glEnd();

    // Clean-up
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  
  void DrawText(double x, double y, const char* text, StringAlignment alignment)
  {
    void* font = GLUT_BITMAP_HELVETICA_18;
    float xPos = 0;
    float yPos = 0;

    switch (alignment) {
      case eAlignRight:
        x = gGameData->window.width - x - StringWidth(font, text);
        break;
      case eAlignCenter:
        x = (gGameData->window.width - StringWidth(font, text)) / 2.0f;
        break;
      default:
        break;
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, gGameData->window.width, 0, gGameData->window.height, 0, 4);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x, y, kTextZ);

    glColor3f(0.1, 0.1, 0.1);
    for (char* ch = const_cast<char*>(text); *ch != '\0'; ++ch) {
      glRasterPos2f(xPos, yPos);
      switch (*ch) {
        case '\n':
          xPos = 0;
          yPos -= kCharHeight;
          break;
        default:
          glutBitmapCharacter(font, *ch);
          xPos +=glutBitmapWidth(font, *ch);
          break;
      }
    }
    glColor3f(1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }


  float StringWidth(void* font, const char* text)
  {
    float maxWidth = 0;

    float width = 0;
    for (char* ch = const_cast<char*>(text); *ch; ++ch) {
      switch (*ch) {
        case '\n':
          if (width > maxWidth)
            maxWidth = width;
          width = 0;
          break;
        default:
          width += glutBitmapWidth(font, *ch);
          break;
      }
    }
    if (width > maxWidth)
      maxWidth = width;
    return maxWidth;
  }


  bool CheckGLError(const char *errMsg)
  {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
      fprintf(stderr, "%s: %s (%d)\n", errMsg, gluErrorString(err), err);
    return (err != GL_NO_ERROR);
  }


} // namespace cat

