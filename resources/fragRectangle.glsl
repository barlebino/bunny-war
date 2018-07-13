#version 330 core

uniform sampler2D texCol;

in vec2 vert_texCoord;

out vec4 color;

void main() {
  color = texture(texCol, vert_texCoord);
  color = vec4(color.r * .5, color.g * .5, color.b *.5, color.a);
  if(color.a < .1) {
    color = vec4(.75, 0.46, 0.46, 1.0);
    //discard;
  }
}
