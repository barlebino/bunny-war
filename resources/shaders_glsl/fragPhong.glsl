#version 330 core

struct Material {
  sampler2D diffuse;
};

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  // Attenuation
  float constant;
  float linear;
  float quadratic;
};

in vec3 frag_nor;
in vec3 frag_pos;
in vec2 frag_tex_coord;

out vec4 out_color;

uniform Material material;
uniform Light light;

void main() {
  // Calculate Attenuation
  float distance = length(light.position - frag_pos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + 
    light.quadratic * (distance * distance)); 
  // Ambient
  vec3 ambient = light.ambient *
    texture(material.diffuse, frag_tex_coord).rgb;
  // Diffuse
  vec3 norm = normalize(frag_nor);
  vec3 lightDir = normalize(light.position - frag_pos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse *
    (diff * texture(material.diffuse, frag_tex_coord).rgb);
  // No specular component for now
  // TODO: Flags for enabling/disabling specular component
  // Apply attenuation
  ambient = attenuation * ambient;
  diffuse = attenuation * diffuse;
  // Final
  out_color = vec4((ambient + diffuse), 1.0);
}
