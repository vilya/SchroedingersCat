#include "drawing.h"

#include "image.h"
#include "gamedata.h"
#include "shader.h"

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


  //
  // Types
  //

  struct Shape {
    GLsizei numVerts;
    
    GLuint vertBufferID;
    GLuint textureID;

    GLuint stride;
    GLuint vertexOffset;
    GLuint texCoordOffset;

    Shape();
    ~Shape();
  };


  struct DrawingData {
    Shape playArea;
    char* statusMessage;

    DrawingData();
    ~DrawingData();
  };


  //
  // Forward declarations
  //

  GLuint UploadTexture(Image* img);
  GLuint UploadVerts(int numVerts, float* verts);

  void DrawShape(Shape* shape);


  //
  // Shape public methods
  //

  Shape::Shape() :
    numVerts(0),
    vertBufferID(0),
    textureID(0),
    stride(0),
    vertexOffset(0),
    texCoordOffset(0)
  {
  }


  Shape::~Shape()
  {
    if (vertBufferID)
      glDeleteBuffers(1, &vertBufferID);
  }


  //
  // DrawingData public methods
  //

  DrawingData::DrawingData() :
    playArea(),
    statusMessage(NULL)
  {
    float playAreaVerts[] = {
      // x, y, u, v
      0, 0, 0, 0,
      1, 0, 4, 0,
      1, 1, 4, 4,
      0, 1, 0, 4
    };
    Image* floor = LoadJPEG("resource/floor.jpg");
    assert(floor);

    playArea.numVerts = 4;
    playArea.vertBufferID = UploadVerts(4, playAreaVerts);
    playArea.textureID = UploadTexture(floor);
    playArea.stride = sizeof(playAreaVerts) / playArea.numVerts;
    playArea.vertexOffset = 0;
    playArea.texCoordOffset = sizeof(float) * 2;

    // Clean up.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete floor;
  }


  DrawingData::~DrawingData()
  {
    glDeleteTextures(1, &playArea.textureID);
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

    DrawShape(&game->draw->playArea);
  }


  void DrawBullets(GameData* game)
  {
    // TODO
  }


  void DrawPlayer(GameData* game)
  {
    // TODO
  }


  void DrawEffects(GameData* game)
  {
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


  GLuint UploadVerts(int numVerts, float* verts)
  {
    GLuint bufID;

    glGenBuffers(1, &bufID);
    glBindBuffer(GL_ARRAY_BUFFER, bufID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVerts, verts, GL_STATIC_DRAW);

    // Clean up GL state.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return bufID;
  }


  void DrawShape(Shape* shape)
  {
    //glBindBuffer(GL_ARRAY_BUFFER, shape->vertBufferID);
    glBindTexture(GL_TEXTURE_2D, shape->textureID);

    glEnable(GL_TEXTURE_2D);
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //glVertexPointer(2, GL_FLOAT, shape->stride, (const GLvoid*)shape->vertexOffset);
    //glTexCoordPointer(2, GL_FLOAT, shape->stride, (const GLvoid*)shape->texCoordOffset);

    //glDrawArrays(GL_TRIANGLE_FAN, 0, shape->numVerts);

    glBegin(GL_QUADS);
      glTexCoord2f(0, 0);
      glVertex2f(0, 0);

      glTexCoord2f(4, 0);
      glVertex2f(1, 0);

      glTexCoord2f(4, 4);
      glVertex2f(1, 1);

      glTexCoord2f(0, 4);
      glVertex2f(0, 1);
    glEnd();

    // Clean-up
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

} // namespace cat

