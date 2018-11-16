#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;

// given in view space to the fragment shader
out vec3 frag_nor;
out vec3 frag_pos;
out vec3 tex_coord;
out vec3 frag_pos_light_space;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 lightspace;

void main() {
  tex_coord = vertPos;
  frag_nor = mat3(modelview) * vertNor; // No non-uniform scaling
  frag_pos = (modelview * vec4(vertPos, 1.0)).xyz;
  frag_pos_light_space = (lightspace * vec4(vertPos, 1.0)).xyz;
  gl_Position = projection * modelview * vec4(vertPos, 1.0);
}