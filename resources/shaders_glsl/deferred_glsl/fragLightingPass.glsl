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

vec3 calcDirectionalLight(DirectionalLight directionalLight,
  vec3 viewDir, vec3 nor, vec4 col) {
  vec3 diffuseMapColor = col.rgb;
  // Ambient
  vec3 ambient = directionalLight.ambient * diffuseMapColor / 2;
  // Diffuse
  vec3 lightDir = normalize(-directionalLight.direction);
  float diff = max(dot(nor, lightDir), 0.0);
  vec3 diffuse = directionalLight.diffuse * (diff * diffuseMapColor);
  // Specular
  vec3 reflectDir = reflect(-lightDir, nor);
  vec3 halfwayDir = normalize(viewDir + lightDir);
  float spec = pow(max(dot(halfwayDir, nor), 0.0), col.a);
  vec3 specular = directionalLight.specular * (spec * diffuseMapColor * 2);
  // TODO: Shadows
  return ambient + diffuse + specular;
}

void main() {
  // Access textures once
  vec3 pos = texture(gpos, frag_texCoord).rgb;
  vec3 nor = texture(gnor, frag_texCoord).rgb;
  vec4 col = texture(gcol, frag_texCoord).rgba;
  // Early return
  if(col.a == 0)
    discard;
  // Calculate only once
  vec3 viewDir = normalize(-pos); // Vector from fragment to camera
  // Final light
  vec3 total_light = vec3(0.0, 0.0, 0.0);
  // Add all directional light contributions
  for(int i = 0; i < NUM_DIRECTIONAL_LIGHTS; i++) {
    total_light = total_light +
      calcDirectionalLight(directionalLights[i], viewDir, nor, col);
  }
  // Turn final light into vec4
  color = vec4(total_light, 1.0);
  // TODO: Paste into bright color buffer

  // Output
  /*
  // If not super black in any way, make white
  if(col.a != 0) {
    color = vec4(1.0, 1.0, 1.0, 1.0);
  } else {
    color = vec4(0.0, 0.0, 0.0, 0.0);
  }
  */
}
