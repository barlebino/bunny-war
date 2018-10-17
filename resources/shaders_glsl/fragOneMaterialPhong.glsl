#version 330 core

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct PointLight {
  vec3 position; // Given in view space
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  // Attenuation
  float constant;
  float linear;
  float quadratic;
};

#define NUM_POINT_LIGHTS 3

in vec3 frag_nor;
in vec3 frag_pos;

out vec4 out_color;

uniform Material material;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
// TESTING
uniform bool isBlinn;

vec3 calcPointLight(PointLight pointLight, vec3 norm, vec3 viewDir) {
  // Calculate attenuation
  float distance = length(pointLight.position - frag_pos);
  float attenuation = 1.0 / (pointLight.constant + 
    pointLight.linear * distance + 
    pointLight.quadratic * (distance * distance));
  // Ambient
  vec3 ambient = pointLight.ambient * material.ambient;
  // Diffuse
  vec3 lightDir = normalize(pointLight.position - frag_pos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = pointLight.diffuse * (diff * material.diffuse);
  // Specular
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec;
  // TESTING
  if(isBlinn) {
    vec3 halfwayDir = normalize(viewDir + lightDir);
    spec = pow(max(dot(halfwayDir, norm), 0.0), material.shininess);
  } else {
    //spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    spec = pow(dot(viewDir, reflectDir) * 0.5 + 0.5, material.shininess);
  }
  vec3 specular = pointLight.specular * (spec * material.specular);
  // Apply attenuation
  ambient = attenuation * ambient;
  diffuse = attenuation * diffuse;
  specular = attenuation * specular;
  // Final
  return ambient + diffuse + specular;
}

void main() {
  // Final light
  vec3 total_light = vec3(0.0, 0.0, 0.0);
  // Constant across all lighting calculations
  vec3 norm = normalize(frag_nor);
  vec3 viewDir = normalize(-frag_pos); // Vector from fragment to camera
  // Add all point light contributions
  // TODO: change all "i < 3" to "i < NUM_POINT_LIGHTS" in shaders
  for(int i = 0; i < NUM_POINT_LIGHTS; i++) {
    total_light = total_light +
      calcPointLight(pointLights[i], norm, viewDir);
  }
  // Turn final light into vec4
  out_color = vec4(total_light, 1.0);
}
