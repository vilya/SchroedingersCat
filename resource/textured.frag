#version 300

uniform sampler2D tex;

in vec2 varyingTexPos;

out vec4 fragColor;

void main()
{
  vec4 texColor = texture2D(tex, varyingTexPos);
  texColor.rgb *= texColor.a;
  fragColor = texColor;
}
