#include "level.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <sstream>

namespace cat {

  //
  // Level public methods
  //

  Level::Level() :
    name(),
    duration(0),
    maxAtomCount(0),
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


  void Level::randomise(int numAtoms, double emitFrequency, double maxSpeed, double minSpeed)
  {
    const std::string adjective[] = {
      "Random",
      "Quantum",
      "Imaginary",
      "Strange",
      "Weird",
      "Happy",
      "Unconventional"
    };
    const std::string noun[] = {
      "Level",
      "World",
      "Science",
      "Feline",
      "Stuff",
      "Cat",
      "Experiment"
    };
    const int kNumAdjectives = 7;
    const int kNumNouns = 7;
    const double kNormalThresh = 0.95;
    const double kSuperpositionThresh = 0.99;

    int adjectiveIndex = int(drand48() * kNumAdjectives) % kNumAdjectives;
    int nounIndex = int(drand48() * kNumNouns) % kNumNouns;
    std::ostringstream buf;
    buf << adjective[adjectiveIndex] << " " << noun[nounIndex];
    
    while (numAtoms <= 0)
      numAtoms = int(drand48() * 64) % 64;

    while (emitFrequency <= 0)
      emitFrequency = drand48() * 1000.0;

    name = buf.str();
    duration = emitFrequency * numAtoms + 5000.0;
    maxAtomCount = 0;
    
    for (int i = 0; i < numAtoms; ++i) {
      double typeVal = drand48();
      AtomType type = eAtomNormal;
      if (typeVal > kNormalThresh)
        type = (typeVal > kSuperpositionThresh) ? eAtomEntanglement : eAtomSuperposition;

      // Random emit position along any wall.
      int wall = int(drand48() * 4) % 4;
      float angle = (drand48() * 0.9 + 0.05) * M_PI; // in radians, 0 is parallel to +ve x axis, pi/2 is +ve y axis
      float speed = drand48() * (maxSpeed - minSpeed) + minSpeed;

      Vec2 pos;
      switch (wall) {
      case 0: // left wall
        pos = Vec2(0, drand48());
        angle += M_PI_2;
        break;
      case 1: // top wall
        pos = Vec2(drand48(), 1);
        angle += M_PI;
        break;
      case 2: // right wall
        pos = Vec2(1, drand48());
        angle -= M_PI_2;
        break;
      case 3: // bottom wall
        pos = Vec2(drand48(), 0);
        break;
      }

      Vec2 vel = Vec2(cos(angle), sin(angle)) * speed;

      addAtom(type, (i + 1) * emitFrequency, pos, vel);
    }
  }


  void Level::startLevel()
  {
    atomCount = 0;
    std::copy(launchPosition, launchPosition + maxAtomCount, position);
    std::copy(launchVelocity, launchVelocity + maxAtomCount, velocity);
  }


  //
  // Functions
  //
  
  bool LoadLevels(const char* filename, std::list<Level>& levels)
  {
    FILE* f = fopen(filename, "r");
    if (!f)
      return false;

    levels.clear();

    enum ParserState {
      eParserInitial,
      eParserDuration,
      eParserAtoms,
      eParserError
    };
    ParserState state = eParserInitial;

    char atomTypeChr;
    double atomSpawnTime;
    Vec2 atomPos;
    Vec2 atomVel;

    char line[1024];
    int lineNum = 0;
    while (fgets(line, 1024, f)) {
      ++lineNum;

      // Trim off trailing whitespace.
      size_t len = strlen(line);
      while (len > 0 && isspace(line[len - 1]))
        --len;
      line[len] = '\0';
      
      if (len == 0) {
        state = eParserInitial;
        continue;
      }

      switch (state) {
      case eParserInitial:
        levels.push_back(Level());
        levels.back().name = line;
        state = eParserDuration;
        break;

      case eParserDuration:
        if (!strncmp(line, "Duration ", strlen("Duration ")) != 0)
          state = eParserError;
        else if (sscanf(line, "Duration %lf", &levels.back().duration) != 1)
          state = eParserError;
        else
          state = eParserAtoms;
        break;

      case eParserAtoms:
        if (sscanf(line, "%c %lf %lf %lf %lf %lf", &atomTypeChr, &atomSpawnTime, &atomPos.x, &atomPos.y, &atomVel.x, &atomVel.y) != 6)
          state = eParserError;
        else if (atomTypeChr == 'A')
          levels.back().addAtom(eAtomNormal, atomSpawnTime, atomPos, atomVel);
        else if (atomTypeChr == 'S')
          levels.back().addAtom(eAtomSuperposition, atomSpawnTime, atomPos, atomVel);
        else if (atomTypeChr == 'E')
          levels.back().addAtom(eAtomEntanglement, atomSpawnTime, atomPos, atomVel);
        else
          state = eParserError;

      case eParserError:
        break;
      }

      if (state == eParserError) {
        fprintf(stderr, "%s, line %d: syntax error.\n", filename, lineNum);
        break;
      }
    }

    fclose(f);
    if (state == eParserDuration) {
      fprintf(stderr, "%s, line %d: incomplete level, missing duration.\n", filename, lineNum);
      return false;
    }
    else {
      return true;
    }
  }

} // namespace cat

