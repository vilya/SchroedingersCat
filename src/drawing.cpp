#include "drawing.h"

#include "image.h"
#include "gamedata.h"

#include <cassert>
#include <cstdlib>

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


  //
  // Types
  //

  struct DrawingData {
    GLuint floorTextureID;
    GLuint playerTextureID;

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

  float StringWidth(void* font, const char* text);


  //
  // DrawingData public methods
  //

  DrawingData::DrawingData() :
    floorTextureID(0),
    playerTextureID(0),
    statusMessage(NULL)
  {
    // Load the floor texture.
    Image* floor = LoadJPEG("resource/floor.jpg");
    assert(floor);
    floorTextureID = UploadTexture(floor);
    delete floor;

    // Load the player texture.
    // Image* player = LoadJPEG("resource/player.jpg");
    // assert(player);
    // playerTextureID = UploadTexture(player);
    // delete player;
  }


  DrawingData::~DrawingData()
  {
    if (floorTextureID)
      glDeleteTextures(1, &floorTextureID);
    if (playerTextureID)
      glDeleteTextures(1, &playerTextureID);
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

    DrawTiledQuad(0, 0, kFloorZ, 1, 1, game->draw->floorTextureID, 4, 4);
  }


  void DrawPlayer(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);

    PlayerData& player = game->player;
    DrawingData* draw = game->draw;

    Vec2 bottomLeft = player.position - player.size / 2.0;
    DrawTiledQuad(bottomLeft.x, bottomLeft.y, kPlayerZ, player.size.x, player.size.y,
                  draw->playerTextureID, 1, 1);
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

    glBegin(GL_POINTS);
    for (unsigned int i = 0; i < bullets.count; ++i) {
      glVertex3d(bullets.position[i].x, bullets.position[i].y, kBulletZ);
    }
    glEnd();
  }


  void DrawEffects(GameData* game)
  {
    assert(game != NULL);
    assert(game->draw != NULL);
    // TODO
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
    float charHeight = 21;
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

    for (char* ch = const_cast<char*>(text); *ch != '\0'; ++ch) {
      glRasterPos2f(xPos, yPos);
      switch (*ch) {
        case '\n':
          xPos = 0;
          yPos -= charHeight;
          break;
        default:
          glutBitmapCharacter(font, *ch);
          xPos +=glutBitmapWidth(font, *ch);
          break;
      }
    }

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

