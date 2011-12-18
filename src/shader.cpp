#include "shader.h"

#include <cstdarg>
#include <cstdio>

namespace cat {

  //
  // Constants
  //

  const Shader kInvalidShader = 0;
  const Program kInvalidProgram = 0;


  //
  // Functions
  //

  Shader LoadShaderFile(ShaderType shaderType, const char* path)
  {
    FILE* shaderFile = fopen(path, "r");
    if (shaderFile == NULL) {
      fprintf(stderr, "Unable to load shader: couldn't open file %s\n", path);
      return kInvalidShader;
    }

    // Get the length of the file.
    fseek(shaderFile, 0, SEEK_END);
    size_t length = ftell(shaderFile);
    fseek(shaderFile, 0, SEEK_SET);

    // Allocate a buffer large enough to hold the entire file and read it in.
    char* text = new char[length + 1];
    fread(text, sizeof(const char), length, shaderFile);
    text[length] = '\0';
    fclose(shaderFile);

    // Create the shader.
    Shader shader = LoadShaderString(shaderType, text, path);

    // Clean up.
    delete[] text;
    return shader;
  }


  Shader LoadShaderString(ShaderType shaderType, const char* text,
      const char* path)
  {
    const GLenum kGLShaderTypes[] = {
      GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
    };
    GLenum type = kGLShaderTypes[shaderType];

    Shader shader = glCreateShader(type);
    if (!shader) {
      AssertNoGLError("failed to create shader");
      return kInvalidShader;
    }

    glShaderSource(shader, 1, (const GLchar**)&text, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
      if (path)
        fprintf(stderr, "Shader %s failed to compile:\n", path);
      else
        fprintf(stderr, "Shader failed to compile\n");
      PrintShaderInfoLog(shader);
      shader = kInvalidShader;
    }

    return shader;
  }


  Program LinkShader(Shader vertexShader, Shader fragmentShader, ...)
  {
    if (!vertexShader || !fragmentShader)
      return kInvalidProgram;

    Program program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    va_list attrList;
    va_start(attrList, fragmentShader);
    int index = 0;
    char* name;
    while ((name = va_arg(attrList, char*)) != NULL) {
      glBindAttribLocation(program, index, name);
      ++index;
    }

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
      fprintf(stderr, "Shader program failed to link.\n");
      PrintProgramInfoLog(program);
      program = kInvalidProgram;
    }

    // Mark shaders for deletion when this program is freed.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
  }


  void PrintShaderInfoLog(Shader shader)
  {
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0) {
      infoLog = new char[infologLength];
      glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
      fprintf(stderr, "%s\n", infoLog);
      delete[] infoLog;
    }
  }


  void PrintProgramInfoLog(Program program)
  {
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0) {
      infoLog = new char[infologLength];
      glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
      fprintf(stderr, "%s\n", infoLog);
      delete[] infoLog;
    }
  }


  bool AssertNoGLError(const char* errMsg)
  {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      fprintf(stderr, "%s: %s (%d)\n", errMsg, gluErrorString(err), err);
      return false;
    }
    return true;
  }

} // namespace cat
