#version 330 core
layout (location = 0) in vec3 vertPos;

uniform mat4 modelviewproj;

void main() {
  gl_Position = modelviewproj * vec4(vertPos, 1.0);
}