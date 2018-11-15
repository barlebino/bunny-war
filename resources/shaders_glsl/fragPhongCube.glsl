#version 330 core

struct Material {
  samplerCube diffuse;
  samplerCube specular;
  float shininess;
};

struct PointLight {
  vec3 position; // View space
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

in vec3 frag_nor;
in vec3 frag_pos;
in vec3 tex_coord; // samplerCube query
in vec3 frag_pos_light_space;
// all points in frag_pos_light_space perfect [-1, 1] cube
// assuming ortho. TODO: not sure about proj yet

out vec4 out_color;

uniform Material material;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform DirectionalLight directionalLights[NUM_DIRECTIONAL_LIGHTS];
uniform sampler2D shadowMap;

// TODO: frag_pos_light_space ony for directionalLights[0]
float calcShadow(vec3 frag_pos_light_space) {
  // TODO: perspective divide w/ vec4 frag_pos_light_space
  vec3 projCoords = frag_pos_light_space * 0.5 + 0.5; // [-1, 1] -> [0, 1]
  float closestDepth = texture(shadowMap, projCoords.xy).r;
  float currentDepth = projCoords.z;
  float bias = max(
    0.05 * (
      1.0 - dot(
        frag_nor, 
        normalize(-directionalLights[0].direction)
      )
    ),
    0.005
  );
  // 0 if behind closestDepth, 1 if is/in front of closestDepth
  // currentDepth is behind closestDepth if currentDepth > closestDepth
  float shadow = currentDepth > closestDepth + bias ? 0.0 : 1.0;
  return shadow;
}

vec3 calcDirectionalLight(DirectionalLight directionalLight,
  vec3 norm, vec3 viewDir) {
  vec3 diffuseMapColor = texture(material.diffuse, tex_coord).rgb;
  // Ambient
  vec3 ambient = directionalLight.ambient *  diffuseMapColor;
  // Diffuse
  vec3 lightDir = normalize(-directionalLight.direction);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = directionalLight.diffuse * (diff * diffuseMapColor);
  // Specular
  vec3 reflectDir = reflect(-lightDir, norm);
  vec3 halfwayDir = normalize(viewDir + lightDir);
  float spec = pow(max(dot(halfwayDir, norm), 0.0), material.shininess);
  vec3 specular = directionalLight.specular *
    (spec * texture(material.specular, tex_coord).rgb);
  // Final
  // TODO: One frag_pos_light_space per directional light
  return ambient + (diffuse + specular) * calcShadow(frag_pos_light_space);
}

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
  vec3 halfwayDir = normalize(viewDir + lightDir);
  float spec = pow(max(dot(halfwayDir, norm), 0.0), material.shininess);
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
  // Add all direction light contributions
  for(int i = 0; i < NUM_DIRECTIONAL_LIGHTS; i++) {
    total_light = total_light +
      calcDirectionalLight(directionalLights[i], norm, viewDir);
  }
  // Turn final light into vec4
  out_color = vec4(total_light, 1.0);
}
