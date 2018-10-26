#version 330 core

struct Material {
  sampler2D diffuse;
};

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
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

#define NUM_POINT_LIGHTS 3
#define NUM_DIRECTIONAL_LIGHTS 1

in vec3 frag_nor;
in vec3 frag_pos;
in vec2 frag_tex_coord;

out vec4 out_color;

uniform Material material;
uniform PointLight light;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform DirectionalLight directionalLights[NUM_DIRECTIONAL_LIGHTS];

vec3 calcDirectionalLight(DirectionalLight directionalLight,
  vec3 norm) { // no need for viewDir
  vec3 diffuseMapColor = texture(material.diffuse, frag_tex_coord).rgb;
  // Ambient
  vec3 ambient = directionalLight.ambient *  diffuseMapColor;
  // Diffuse
  vec3 lightDir = normalize(-directionalLight.direction);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = directionalLight.diffuse * (diff * diffuseMapColor);
  // Final
  return ambient + diffuse;
}

vec3 calcPointLight(PointLight pointLight, vec3 norm) { // no need for viewDir
  vec3 diffuseMapColor = texture(material.diffuse, frag_tex_coord).rgb;
  // Calculate attenuation
  float distance = length(pointLight.position - frag_pos);
  float attenuation = 1.0 / (pointLight.constant + 
    pointLight.linear * distance + 
    pointLight.quadratic * (distance * distance));
  // Ambient
  vec3 ambient = pointLight.ambient * diffuseMapColor;
  // Diffuse
  vec3 lightDir = normalize(pointLight.position - frag_pos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = pointLight.diffuse * (diff * diffuseMapColor);
  // Apply attenuation
  ambient = attenuation * ambient;
  diffuse = attenuation * diffuse;
  // Final
  return ambient + diffuse;
}

void main() {
  // Final light
  vec3 total_light = vec3(0.0, 0.0, 0.0);
  // Constant across all lighting calculations
  vec3 norm = normalize(frag_nor);
  // Add all point light contributions
  for(int i = 0; i < NUM_POINT_LIGHTS; i++) {
    total_light = total_light +
      calcPointLight(pointLights[i], norm);
  }
  // Add all directional light contributions
  for(int i = 0; i < NUM_DIRECTIONAL_LIGHTS; i++) {
    total_light = total_light +
      calcDirectionalLight(directionalLights[i], norm);
  }
  // Turn final light into vec4
  out_color = vec4(total_light, 1.0);
}
