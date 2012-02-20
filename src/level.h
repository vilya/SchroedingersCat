#ifndef cat_levels_h
#define cat_levels_h

#include "vec2.h"

#include <list>
#include <string>

namespace cat {

  //
  // Constants
  //

  static const unsigned int kMaxAtoms = 1024;


  //
  // Types
  //

  enum AtomType {
    eAtomNormal,        // Atom which removes 1 life when you hit it.
    eAtomSuperposition, // Atom which adds a superposition powerup when you hit it.
    eAtomEntanglement   // Atom which adds an entanglement powerup when you hit it.
  };


  struct Level {
    // Static level data.
    std::string name;
    double duration;
    unsigned int maxAtomCount;
    AtomType atomType[kMaxAtoms];
    double launchTime[kMaxAtoms];
    Vec2 launchPosition[kMaxAtoms];
    Vec2 launchVelocity[kMaxAtoms];

    // Dynamic level data.
    unsigned int atomCount;
    Vec2 position[kMaxAtoms];
    Vec2 velocity[kMaxAtoms];

    Level();

    // Call this repeatedly to add fixed initial atom data.
    void addAtom(AtomType type, double t, const Vec2& pos, const Vec2& vel);

    // Call this once to generate a random level.
    void randomise(int numAtoms = -1, double emitFrequency = -1.0, double maxSpeed = 0.004, double minSpeed = 0.001);

    // Reset the dynamic data, ready to play the level again.
    void startLevel();
  };


  //
  // Functions
  //

  bool LoadLevels(const char* filename, std::list<Level>& levels);

} // namespace cat

#endif // cat_levels_h

