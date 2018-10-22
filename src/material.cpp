#include "material.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Only ambient and diffuse will be affected by gamma correction

Material emerald = { // TODO: Does compiler calculate these?
  glm::pow(
    glm::vec3(0.0215f, 0.1745f, 0.0215f),
    glm::vec3(2.2f, 2.2f, 2.2f)
  ),
  glm::pow(
    glm::vec3(0.07568f, 0.61424f, 0.07568f),
    glm::vec3(2.2f, 2.2f, 2.2f)
  ),
  glm::vec3(0.633f, 0.727811f, 0.633f),
  .6f * 128.f
};

Material jade = {
  glm::pow(
    glm::vec3(0.135f, 0.2225f, 0.1575f),
    glm::vec3(2.2f, 2.2f, 2.2f)
  ),
  glm::pow(
    glm::vec3(0.54f, 0.89f, 0.63f),
    glm::vec3(2.2f, 2.2f, 2.2f)
  ),
  glm::vec3(0.316228f, 0.316228f, 0.316228f),
  .1f * 128.f
};

// Ambient and diffuse only affected by gamma correction

Material copper = {
  glm::pow(
    glm::vec3(0.19125f, 0.0735f, 0.0225f),
    glm::vec3(2.2f, 2.2f, 2.2f)
  ),
  glm::pow(
    glm::vec3(0.7038f, 0.27048f, 0.0828f),
    glm::vec3(2.2f, 2.2f, 2.2f)
  ),
  glm::vec3(0.256777f, 0.137622f, 0.086014f),
  .1f * 128.f
};
