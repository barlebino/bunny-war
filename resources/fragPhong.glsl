#version 330 core

in vec3 fragNor;

out vec4 out_color;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_pos;

void main() {
  //out_color = vec4(object_color * light_color, 1.0);
  out_color = vec4(fragNor, 1.0);
}
