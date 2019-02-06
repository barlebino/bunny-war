#version 330 core

// TODO: If no specular, set to 0
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
in vec3 frag_pos_light_space;
// all points in frag_pos_light_space perfect [-1, 1] cube
// assuming ortho. TODO: not sure about proj yet

layout (location = 0) out vec4 out_color;
layout (location = 1) out vec4 bright_color;

uniform Material material;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform DirectionalLight directionalLights[NUM_DIRECTIONAL_LIGHTS];
uniform sampler2D shadowMap;

// TODO: frag_pos_light_space ony for directionalLights[0]
float calcShadow(vec3 frag_pos_light_space) {
  // TODO: perspective divide w/ vec4 frag_pos_light_space
  vec3 projCoords = frag_pos_light_space * 0.5 + 0.5; // [-1, 1] -> [0, 1]
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
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  // Average the 9 surrounding depth texels
  for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
      float closestDepth = texture(shadowMap,
        projCoords.xy + vec2(x, y) * texelSize).r;
      shadow = shadow +
        (currentDepth > closestDepth + bias ? 0.0 : 1.0);
    }
  }
  shadow = shadow / 9.0;
  return shadow;
}

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
  // TODO: One frag_pos_light_space per directional light
  return ambient + diffuse * calcShadow(frag_pos_light_space);
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
  // bright color filter from learnopengl
  float brightness = dot(out_color.rgb, vec3(0.2126, 0.7152, 0.0722));
  if(brightness > 1.0) {
    bright_color = vec4(out_color.rgb, 1.0);
  } else {
    bright_color = vec4(0.0, 0.0, 0.0, 1.0);
  }
}
