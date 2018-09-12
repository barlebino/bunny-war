#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;

out vec3 fragNor;

uniform mat4 perspective;
uniform mat4 placement;

void main() {
  fragNor = vertNor;
  gl_Position = perspective * placement * vec4(vertPos, 1.0);
}