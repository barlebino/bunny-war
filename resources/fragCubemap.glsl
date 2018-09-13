#version 330 core

uniform samplerCube skybox;

in vec3 vert_texCoord;

out vec4 color;

void main() {
  color = texture(skybox, vert_texCoord);
}