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

void main() {
  // Calculate attenuation
  float distance = length(pointLights[0].position - frag_pos);
  float attenuation = 1.0 / (pointLights[0].constant + 
    pointLights[0].linear * distance + 
    pointLights[0].quadratic * (distance * distance)); 
  // Ambient
  vec3 ambient = pointLights[0].ambient * material.ambient;
  // Diffuse
  vec3 norm = normalize(frag_nor);
  vec3 lightDir = normalize(pointLights[0].position - frag_pos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = pointLights[0].diffuse * (diff * material.diffuse);
  // Specular
  vec3 viewDir = normalize(-frag_pos); // Vector from fragment to camera
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = pointLights[0].specular * (spec * material.specular);  
  // Apply attenuation
  ambient = attenuation * ambient;
  diffuse = attenuation * diffuse;
  specular = attenuation * specular;
  // Final
  out_color = vec4((ambient + diffuse + specular), 1.0);
}
