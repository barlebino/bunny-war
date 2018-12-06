#version 330 core

uniform sampler2D texCol;
uniform float exposure;

in vec2 vert_texCoord;

out vec4 color;

void main() {
  // Default
  vec4 texColor = texture(texCol, vert_texCoord);
  if(texColor.a < 0.1) {
    discard;
  }
  vec3 hdrColor = texColor.rgb;
  vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
  color = vec4(mapped, 1.0);
}
