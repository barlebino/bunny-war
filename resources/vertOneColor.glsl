#version 330 core

layout(location = 0) in vec3 vertPos;

uniform mat4 perspective;
uniform mat4 placement;

void main() {
  gl_Position = perspective * placement * vec4(vertPos, 1.0);
}