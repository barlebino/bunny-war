#version 330 core

uniform sampler2D texCol;

in vec2 vert_texCoord;

out vec4 color;

void main() {
  vec4 texColor = texture(texCol, vert_texCoord);
  if(texColor.a < 0.1) {
    discard;
  }
  color = texColor;
}
