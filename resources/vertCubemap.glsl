#version 330 core

layout (location = 0) in vec3 vertPos;

out vec3 vert_texCoord;

uniform mat4 perspective;
uniform mat4 placement;

void main() {
  vert_texCoord = vertPos;
  gl_Position = perspective * placement * vec4(vertPos, 1.0);
}
