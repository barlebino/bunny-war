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
uniform sampler2D shadowMap; // only for directional light
uniform mat4 viewToLight; // view space to light space

in vec2 frag_texCoord;
out vec4 color;

// position given in view space
float calcShadow(vec3 pos, vec3 nor) {
  vec3 pos_light_space = (viewToLight * vec4(pos, 1.0)).rgb;
  // TODO? : perspective divide w/ vec4 pos_light_space
  vec3 projCoords = pos_light_space * 0.5 + 0.5; // [-1, 1] -> [0, 1]
  float currentDepth = projCoords.z;
  float bias = max(
    0.05 * (
      1.0 - dot(
        nor, 
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
  vec3 viewDir, vec3 pos, vec3 nor, vec4 col) {
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
  //return ambient + diffuse + specular;
  return ambient + (diffuse + specular) * calcShadow(pos, nor);
}

void main() {
  // Access textures once
  vec3 pos = texture(gpos, frag_texCoord).rgb;
  vec3 nor = texture(gnor, frag_texCoord).rgb;
  vec4 col = texture(gcol, frag_texCoord).rgba;
  // TESTING
  float val = calcShadow(pos, nor);
  pos = pos + vec3(val, val, val) - vec3(val, val, val);
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
      calcDirectionalLight(directionalLights[i], viewDir, pos, nor, col);
  }
  // Turn final light into vec4
  color = vec4(total_light, 1.0);
  // TODO: Paste into bright color buffer
}
