#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;

out vec3 frag_nor;
out vec3 frag_pos;

uniform mat4 modelview;
uniform mat4 projection;

void main() {
  frag_nor = mat3(modelview) * vertNor; // No non-uniform scaling
  frag_pos = (modelview * vec4(vertPos, 1.0)).xyz;
  gl_Position = projection * modelview * vec4(vertPos, 1.0);
}