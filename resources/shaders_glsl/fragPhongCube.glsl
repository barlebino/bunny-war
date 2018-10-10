#version 330 core

struct Material {
  samplerCube diffuse;
  samplerCube specular;
  float shininess;
};

struct PointLight {
  vec3 position;
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
in vec3 tex_coord; // samplerCube query

out vec4 out_color;

uniform Material material;
uniform PointLight pointLights[NUM_POINT_LIGHTS];

vec3 calcPointLight(PointLight pointLight, vec3 norm, vec3 viewDir) {
  // Calculate attenuation
  float distance = length(pointLight.position - frag_pos);
  float attenuation = 1.0 / (pointLight.constant + 
    pointLight.linear * distance + 
    pointLight.quadratic * (distance * distance));
  // Ambient
  vec3 ambient = pointLight.ambient *
    texture(material.diffuse, tex_coord).rgb;
  // Diffuse
  vec3 lightDir = normalize(pointLight.position - frag_pos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = pointLight.diffuse *
    (diff * texture(material.diffuse, tex_coord).rgb);
  // Specular
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = pointLight.specular *
    (spec * texture(material.specular, tex_coord).rgb);
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
  vec3 viewDir = normalize(-frag_pos); // vector from fragment to camera
  // Add all point light contributions
  for(int i = 0; i < NUM_POINT_LIGHTS; i++) {
    total_light = total_light +
      calcPointLight(pointLights[i], norm, viewDir);
  }
  out_color = vec4(total_light, 1.0);
}
