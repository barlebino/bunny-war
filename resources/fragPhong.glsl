#version 330 core

in vec3 frag_nor;
in vec3 frag_pos;

out vec4 out_color;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 cam_pos;

void main() {
  // Ambient
  float ambientStrength = 0.1;
  vec3 ambient = ambientStrength * light_color;
  // Diffuse
  vec3 norm = normalize(frag_nor);
  vec3 lightDir = normalize(light_pos - frag_pos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * light_color;
  // Specular
  float specularStrength = 0.5;
  vec3 viewDir = normalize(cam_pos - frag_pos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * light_color;  
  // Final
  out_color = vec4((ambient + diffuse + specular) * object_color, 1.0);
}
