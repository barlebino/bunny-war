#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 texCoord;

out vec3 frag_nor;
out vec3 frag_pos;
out vec2 frag_tex_coord;
out vec3 frag_pos_light_space;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 lightspace; // modelviewproj of light space transform

void main() {
  frag_nor = mat3(modelview) * vertNor; // No non-uniform scaling
  frag_pos = (modelview * vec4(vertPos, 1.0)).xyz;
  frag_tex_coord = texCoord;
  frag_pos_light_space = (lightspace * vec4(vertPos, 1.0)).xyz;
  gl_Position = projection * modelview * vec4(vertPos, 1.0);
}