#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
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

vec2 convertTexCoord(vec3 texCoord) {
  vec3 absCoord = vec3(abs(texCoord.x), abs(texCoord.y), abs(texCoord.z));
  if(absCoord.x >= absCoord.y && absCoord.x >= absCoord.z) {
    return vec2(texCoord.y / texCoord.x, texCoord.z / texCoord.x);
  } else if(absCoord.y >= absCoord.x && absCoord.y >= absCoord.z) {
    return vec2(texCoord.x / texCoord.y, texCoord.z / texCoord.y);
  } else if(absCoord.z >= absCoord.x && absCoord.z >= absCoord.y) {
    return vec2(texCoord.x / texCoord.z, texCoord.y / texCoord.z);
  } else {
    return vec2(0.0, 0.0);
  }
}

void main() {
  vec2 texCoord2D = (convertTexCoord(tex_coord) + vec2(1.0, 1.0)) / 2.0;
  // Ambient
  vec3 ambient = light.ambient *
    texture(material.diffuse, texCoord2D).rgb;
  // Diffuse
  vec3 norm = normalize(frag_nor);
  vec3 lightDir = normalize(light.position - frag_pos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse *
    (diff * texture(material.diffuse, texCoord2D).rgb);
  // Specular
  vec3 viewDir = normalize(-frag_pos); // Vector from fragment to camera
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular *
    (spec * texture(material.specular, texCoord2D).rgb);
  // Final
  out_color = vec4(ambient + diffuse + specular, 1.0);
}
