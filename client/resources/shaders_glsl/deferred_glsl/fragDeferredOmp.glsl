#version 330 core

// TODO: More complex materials in deferred shading
struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

in vec3 frag_nor;
in vec3 frag_pos;

layout (location = 0) out vec4 pos_color;
layout (location = 1) out vec4 nor_color;
layout (location = 2) out vec4 col_color;

uniform Material material;

void main() {
  pos_color = vec4(frag_pos, 1.0);
  nor_color = vec4(frag_nor, 1.0);
  col_color = vec4(material.diffuse, material.shininess);
}
