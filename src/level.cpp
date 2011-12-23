#include "level.h"

#include <algorithm>

namespace cat {

  //
  // Level public methods
  //

  Level::Level() :
    name(),
    duration(0),
    maxAtomCount(0),
    startTime(0),
    endTime(0),
    atomCount(0)
  {
    std::fill(atomType, atomType + kMaxAtoms, eAtomNormal);
    std::fill(launchTime, launchTime + kMaxAtoms, 0.0);
    std::fill(launchPosition, launchPosition + kMaxAtoms, Vec2());
    std::fill(launchVelocity, launchVelocity + kMaxAtoms, Vec2(1, 0));

    std::fill(position, position + kMaxAtoms, Vec2());
    std::fill(velocity, velocity + kMaxAtoms, Vec2());
  }


  void Level::addAtom(AtomType type, double t, const Vec2& pos, const Vec2& vel)
  {
    if (maxAtomCount == kMaxAtoms)
      return;

    atomType[maxAtomCount] = type;
    launchTime[maxAtomCount] = t;
    launchPosition[maxAtomCount] = pos;
    launchVelocity[maxAtomCount] = vel;
    ++maxAtomCount;
  }


  void Level::startLevel(double gameTime)
  {
    startTime = gameTime;
    endTime = gameTime + duration;
    atomCount = 0;
    std::copy(launchPosition, launchPosition + maxAtomCount, position);
    std::copy(launchVelocity, launchVelocity + maxAtomCount, velocity);
  }


  //
  // Functions
  //

  bool RandomLevel(Level& level)
  {
    // TODO
    return false;
  }


  bool LoadLevel(Level& level)
  {
    // TODO
    return false;
  }

} // namespace cat

