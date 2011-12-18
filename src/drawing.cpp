#include "drawing.h"

#include "image.h"
#include "gamedata.h"

#include <cassert>
#include <cstdlib>
#include <cstdio>

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>

namespace cat {

  //
  // Constants
  //

  static const GLuint kVertexAttrib = 0;
  static const GLuint kTexCoordAttrib = 1;

  static const float kFloorZ = -1;
  static const float kPlayerZ = 0;
  static const float kBulletZ = -0.5;
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

    char* statusMessage;

    DrawingData();
    ~DrawingData();
  };


  //
  // Forward declarations
  //

  GLuint UploadTexture(Image* img);
  GLuint UploadVerts(int numVerts, float* verts);

  void DrawTiledQuad(double x, double y, double z, double w, double h,
                     GLuint textureID, double horizTiles, double vertTiles);
  void DrawText(double x, double y, const char* text, StringAlignment alignment);

  float StringWidth(void* font, const char* text);


  //
  // DrawingData public methods
  //

  DrawingData::DrawingData() :
    floorTextureID(0),
    particleTextureID(0),
    statusMessage(NULL)
  {
    // Load the floor texture.
    Image* floor = LoadJPEG("resource/floor_alt.jpg");
    assert(floor);
    floorTextureID = UploadTexture(floor);
    delete floor;

    // Load the player textures.
    const char* frontTexturePaths[] = {
      "resource/player_front_nopowerup.jpg",
      "resource/player_front_superposition.jpg",
      "resource/player_front_entangling.jpg",
      "resource/player_front_entanglement.jpg"
    };
    const char* backTexturePaths[] = {
      //"resource/player_back_nopowerup.jpg",
      //"resource/player_back_superposition.jpg",
      //"resource/player_back_entangling.jpg",
      //"resource/player_back_entanglement.jpg"
      "resource/player_back.jpg",
      "resource/player_back.jpg",
      "resource/player_back.jpg",
      "resource/player_back.jpg",
    };
    for (int p = ePowerUpNone; p < ePowerUpCount; ++p) {
      Image* front = LoadJPEG(frontTexturePaths[p]);
      assert(front);
      playerFrontTextureID[p] = UploadTexture(front);
      delete front;

      Image* back = LoadJPEG(backTexturePaths[p]);
      assert(back);
      playerBackTextureID[p] = UploadTexture(back);
      delete back;
    }

    // Load the particle textures.
    Image* particle = LoadJPEG("resource/particle.jpg");
    assert(particle);
    particleTextureID = UploadTexture(particle);
    delete particle;
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
  }


  //
  // Public functions
  //

  void InitDrawing(GameData* game)
  {
    assert(game->draw == NULL);
    game->draw = new DrawingData();
  }


  void DrawPlayArea(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    DrawTiledQuad(0, 0, kFloorZ, 1, 1, game->draw->floorTextureID, 1, 1);
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
    DrawTiledQuad(bottomLeft.x, bottomLeft.y, kPlayerZ, player.size.x, player.size.y, textureID, 1, 1);
  }


  void DrawBullets(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    BulletData& bullets = game->particles;
    WindowData& win = game->window;

    GLfloat bulletSize = win.width * bullets.bulletSize;
    if (bulletSize < 1)
      bulletSize = 1;
    glPointSize(bulletSize);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_POINT_SPRITE);
    glBindTexture(GL_TEXTURE_2D, game->draw->particleTextureID);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

    glBegin(GL_POINTS);
    for (unsigned int i = 0; i < bullets.count; ++i) {
      glVertex3d(bullets.position[i].x, bullets.position[i].y, kBulletZ);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
  }


  void DrawEffects(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);
    // TODO
  }


  void DrawHUD(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    WindowData& win = game->window;

    float top = win.height - kCharHeight - 10;
    char msg[1024];

    snprintf(msg, 1024, "Survived %1.2fs", game->gameTime / 1000.0);
    DrawText(10, top, msg, eAlignLeft);

    snprintf(msg, 1024, "%d lives", game->player.livesRemaining);
    DrawText(10, top, msg, eAlignRight);
  }


  void DrawTitles(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    int lines = 6;
    float y = (gGameData->window.height + lines * kCharHeight) / 2.0f;

    DrawText(0, y, "Schroedinger's Cat: The Game", eAlignCenter);
    y -= kCharHeight;
    DrawText(0, y, "(c) Vilya Harvey, 2011", eAlignCenter);
    y -= kCharHeight * 2;
    DrawText(0, y, "Made unofficially for Ludum Dare 22", eAlignCenter);
    y -= kCharHeight * 2;
    DrawText(0, y, "Press [space] to start, [esc] to quit", eAlignCenter);
  }


  void DrawGameOver(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);
    DrawText(0, gGameData->window.height / 2.0f, "GAME OVER\nPress [space] to try again, [esc] to quit", eAlignCenter);
  }


  //
  // Internal functions
  //

  GLuint UploadTexture(Image* img)
  {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int localFormat;
    switch (img->channels()) {
    case 1:
      localFormat = GL_LUMINANCE;
      break;
    case 2:
    case 3:
      localFormat = GL_RGB;
      break;
    default:
      localFormat = GL_RGBA;
      break;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, sizeof(float));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, img->channels(), img->width(), img->height(), 0, localFormat, GL_FLOAT, img->data());

    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
  }


  void DrawTiledQuad(double x, double y, double z, double w, double h, GLuint textureID, double horizTiles, double vertTiles)
  {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
      glTexCoord2f(0, 0);
      glVertex3d(x, y, z);

      glTexCoord2f(horizTiles, 0);
      glVertex3d(x + w, y, z);

      glTexCoord2f(horizTiles, vertTiles);
      glVertex3d(x + w, y + h, z);

      glTexCoord2f(0, vertTiles);
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

    glColor3f(1, 0, 0);
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

} // namespace cat

