#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Material {
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;
};

extern Material emerald;
extern Material jade;
extern Material copper;

#endif