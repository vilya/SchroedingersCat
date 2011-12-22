#ifndef cat_resource_h
#define cat_resource_h

namespace cat {

  //
  // Functions
  //

  // Not threadsafe, for now. Also, path should be a relative path assuming
  // that the resource/ dir is the current dir.
  const char* ResourcePath(const char* path);

} // namespace cat

#endif // cat_resource_h

