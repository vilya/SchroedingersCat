#version 300

uniform mat4 mvpMatrix;

in vec2 pos;
in vec2 texPos;

smooth out vec2 varyingTexPos;

void main()
{
  gl_Position = mvpMatrix * vec4(pos, 0, 1);
  varyingTexPos = texPos;
}
