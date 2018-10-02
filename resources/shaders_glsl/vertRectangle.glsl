#version 330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec2 texCoord;

out vec2 vert_texCoord;

void main() {
  gl_Position = vertPos;
  vert_texCoord = texCoord;
}
