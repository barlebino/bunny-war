#version 330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec2 texCoord;

uniform mat4 perspective;
uniform mat4 placement;

out vec2 vert_texCoord;

void main() {
  gl_Position = perspective * placement * vertPos;
  vert_texCoord = texCoord;
}
