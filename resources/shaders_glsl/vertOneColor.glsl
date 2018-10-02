#version 330 core

layout(location = 0) in vec3 vertPos;

uniform mat4 modelview;
uniform mat4 projection;

void main() {
  gl_Position = projection * modelview * vec4(vertPos, 1.0);
}
