#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 texCoord;

uniform mat4 modelview;
uniform mat4 projection;

out vec2 vert_texCoord;

void main() {
  gl_Position = projection * modelview * vec4(vertPos, 1.0);
  vert_texCoord = texCoord;
}
