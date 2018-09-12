#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;

out vec3 fragNor;

uniform mat4 perspective;
uniform mat4 placement;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  fragNor = vertNor;
  //gl_Position = perspective * placement * vec4(vertPos, 1.0);
  gl_Position = projection * view * model * vec4(vertPos, 1.0);
}