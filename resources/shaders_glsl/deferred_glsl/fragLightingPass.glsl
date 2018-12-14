#version 330 core

struct PointLight {
  vec3 position; // given in view space
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  // Attenuation
  float constant;
  float linear;
  float quadratic;
};

struct DirectionalLight {
  vec3 direction; // Given in view space
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

#define NUM_POINT_LIGHTS 3
#define NUM_DIRECTIONAL_LIGHTS 1

uniform sampler2D gpos;
uniform sampler2D gnor;
uniform sampler2D gcol;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform DirectionalLight directionalLights[NUM_DIRECTIONAL_LIGHTS];
// TODO: Shadows
uniform sampler2D shadowMap;

in vec2 frag_texCoord;
out vec4 color;

void main() {
  // Access textures once
  vec3 pos = texture(gpos, frag_texCoord).rgb;
  vec3 nor = texture(gnor, frag_texCoord).rgb;
  vec4 col = texture(gcol, frag_texCoord).rgba;
  // Output
  // If not super black in any way, make white
  if(col.a != 0) {
    color = vec4(1.0, 1.0, 1.0, 1.0);
  } else {
    color = vec4(0.0, 0.0, 0.0, 0.0);
  }
}
