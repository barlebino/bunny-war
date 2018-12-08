#version 330 core

uniform samplerCube skybox;

in vec3 vert_texCoord;

layout (location = 0) out vec4 out_color;
layout (location = 1) out vec4 bright_color;

void main() {
  out_color = texture(skybox, vert_texCoord);
  // bright color filter from learnopengl
  float brightness = dot(out_color.rgb, vec3(0.2126, 0.7152, 0.0722));
  if(brightness > 1.0) {
    bright_color = vec4(out_color.rgb, 1.0);
  } else {
    bright_color = vec4(0.0, 0.0, 0.0, 1.0);
  }
}