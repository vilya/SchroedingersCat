#ifndef cat_drawing_h
#define cat_drawing_h

namespace cat {

  //
  // Forward type declarations.
  //

  struct DrawingData;
  struct GameData;


  //
  // Functions
  //

  // Call this once before using any of the other drawing functions. It sets up
  // a bunch of vertex buffers, textures, etc. so make sure you call it after
  // the graphics API has been initialised.
  void InitDrawing(GameData* game);

  void DrawPlayArea(GameData* game);
  void DrawBullets(GameData* game);
  void DrawPlayer(GameData* game);
  void DrawEffects(GameData* game);
  
} // namespace cat

#endif // cat_drawing_h
