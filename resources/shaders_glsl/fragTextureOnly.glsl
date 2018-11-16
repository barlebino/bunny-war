#version 330 core

uniform sampler2D texCol;

in vec2 vert_texCoord;

out vec4 color;

void main() {
  /*// Debug depth
  float depthValue = texture(texCol, vert_texCoord).r;
  color = vec4(vec3(depthValue), 1.0);*/

  // Default
  vec4 texColor = texture(texCol, vert_texCoord);
  if(texColor.a < 0.1) {
    discard;
  }
  color = texColor;
}
