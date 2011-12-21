#include "resource.h"

#include <cstdio>

namespace cat {

  //
  // Constants
  //

#ifdef linux
  static const char* kResourcePrefix = "resource";
#else
  static const char* kResourcePrefix = "../Resources";
#endif


  //
  // Functions
  //

  const char* ResourcePath(const char* path)
  {
    static char buf[4096];
    snprintf(buf, 4096, "%s/%s", kResourcePrefix, path);
    return buf;
  }

} // namespace cat

