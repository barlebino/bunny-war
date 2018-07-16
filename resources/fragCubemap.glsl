#version 330 core

uniform samplerCube skybox;

in vec3 vert_texCoord;

out vec4 color;

void main() {
  color = vec4(
    vert_texCoord.r + 1.0,
    vert_texCoord.g + 1.0,
    vert_texCoord.b + 1.0,
    1.0 + 1.0) / 2;
}