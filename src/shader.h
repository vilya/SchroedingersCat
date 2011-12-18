#ifndef cat_shader_h
#define cat_shader_h

#define GL_GLEXT_PROTOTYPES 1
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>

namespace cat {

  //
  // Types
  //

  /// Supported shader types.
  enum ShaderType {
    eInvalidShader = -1,
    eVertexShader,
    eFragmentShader
  };

  /// An identifier for an OpenGL shader.
  typedef unsigned int Shader;

  /// An identifier for an OpenGL program.
  typedef unsigned int Program;


  //
  // Constants
  //

  extern const Shader kInvalidShader;
  extern const Program kInvalidProgram;


  //
  // Functions
  //

  /// Load a shader from a file. The shaderType param specifies whether it's a
  /// vertex shader, fragment shader etc. The path param gives the file to
  /// load it from.
  Shader LoadShaderFile(ShaderType shaderType, const char* path);

  /// Load a shader from a file. The shaderType param specifies whether it's a
  /// vertex shader, fragment shader etc. The str param is the text of the
  /// shader itself. The path param is the name to report if there is an error
  /// in the shader.
  Shader LoadShaderString(ShaderType shaderType, const char* str,
      const char* path = 0);

  /// Link a vertex shader and fragment shader together into a shader program.
  Program LinkShader(Shader vertexShader, Shader fragmentShader, ...);

  /// Print out the contents of the shader info log. Useful for debugging when
  /// your shader won't compile. The obj param is the ID of the shader whose log
  /// you want to print.
  void PrintShaderInfoLog(Shader shader);

  /// Print out the contents of the program info load. Useful for debugging when
  /// your shader program won't link. The obj param is the ID of the program whose
  /// log you want to print.
  void PrintProgramInfoLog(Program program);

  /// Asserts that there hasn't been any OpenGL errors. Will return true if the
  /// assertion holds, false if it doesn't.
  bool AssertNoGLError(const char* errMsg);

} // namespace cat

#endif // cat_shader_h
