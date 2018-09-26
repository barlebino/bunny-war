#version 330 core

struct Material {
  samplerCube diffuse;
  samplerCube specular;
  float shininess;
};

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec3 frag_nor;
in vec3 frag_pos;
in vec3 tex_coord; // samplerCube query

out vec4 out_color;

uniform Material material;
uniform Light light;

void main() {
  // Ambient
  vec3 ambient = light.ambient * texture(material.diffuse, tex_coord).rgb;
  // Diffuse
  vec3 norm = normalize(frag_nor);
  vec3 lightDir = normalize(light.position - frag_pos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse *
    (diff * texture(material.diffuse, tex_coord).rgb);
  // Specular
  vec3 viewDir = normalize(-frag_pos); // Vector from fragment to camera
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular *
    (spec * texture(material.specular, tex_coord).rgb);
  // Final
  out_color = vec4(ambient + diffuse + specular, 1.0);

  // Transform the light to view space
  //vec3 lv_position = light.position - cam_pos;
  /*// Ambient
  vec3 ambient = light.ambient * texture(material.diffuse, tex_coord).rgb;
  // Diffuse
  vec3 norm = normalize(frag_nor);
  vec3 lightDir = normalize(lv_position - frag_pos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse *
    (diff * texture(material.diffuse, tex_coord).rgb);
  // Specular
  //vec3 viewDir = normalize(cam_pos - frag_pos);
  vec3 viewDir = normalize(-frag_pos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular *
    (spec * texture(material.specular, tex_coord).rgb);  
  // Final
  //out_color = vec4((ambient + diffuse + specular), 1.0);
  out_color = vec4((diffuse + diffuse + diffuse), 1.0);*/
  //out_color = texture(material.specular, tex_coord);
  //out_color = vec4(frag_nor, 1.0);
  //out_color = vec4((normalize(frag_nor) +
  //  vec3(1.0, 1.0, 1.0)) / 2.0, 1.0);
  
  //vec3 lightDir = normalize(lv_position - frag_pos);

  //out_color = vec4((normalize(light.position) +
  //  vec3(1.0, 1.0, 1.0)) / 2.0, 1.0);
  //out_color = vec4((normalize(light.position - cam_pos) +
  //  vec3(1.0, 1.0, 1.0)) / 2.0, 1.0);
  //out_color = vec4((normalize(frag_nor) +
  //  vec3(1.0, 1.0, 1.0)) / 2.0, 1.0);
  //out_color = vec4((normalize(frag_pos) +
  //  vec3(1.0, 1.0, 1.0)) / 2.0, 1.0);
  //out_color = vec4((normalize(lv_position) +
  //  vec3(1.0, 1.0, 1.0)) / 2.0, 1.0);
  //out_color = vec4((normalize(lightDir) +
  //  vec3(1.0, 1.0, 1.0)) / 2.0, 1.0);
}
