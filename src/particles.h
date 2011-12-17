#ifndef cat_particles_h
#define cat_particles_h

#include <cstdlib> // for size_t
#include "vec2.h"

namespace cat {

  //
  // Constants
  //

  static const int kMaxParticles = 10000;


  //
  // Types
  //

  struct Particles {
    size_t count;
    Vec2 position[kMaxParticles];
    Vec2 velocity[kMaxParticles];
    double birthday[kMaxParticles];
    unsigned int flags[kMaxParticles];

    Particles();
  };

} // namespace cat

#endif // cat_particles_h

