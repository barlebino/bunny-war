#version 330 core

// learnopengl blur shader
uniform sampler2D texCol;
uniform bool horizontal;
uniform float weight[5] = float[]
  (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

in vec2 vert_texCoord;

out vec4 color;

void main() {
  vec2 tex_offset = 1.0 / textureSize(texCol, 0); // gets size of single texel
  // current fragment's contribution
  vec3 result = texture(texCol, vert_texCoord).rgb * weight[0];
  color = vec4(result, 1.0);
  if(horizontal) {
    for(int i = 1; i < 5; i++) {
      result += texture(texCol, vert_texCoord +
        vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
      result += texture(texCol, vert_texCoord -
        vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
  } else {
    for(int i = 1; i < 5; i++) {
      result += texture(texCol, vert_texCoord +
        vec2(0.0, tex_offset.y * i)).rgb * weight[i];
      result += texture(texCol, vert_texCoord -
        vec2(0.0, tex_offset.y * i)).rgb * weight[i];
    }
  }
  color = vec4(result, 1.0);
}
