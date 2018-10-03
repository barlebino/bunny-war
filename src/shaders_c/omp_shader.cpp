#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "omp_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes ompShader->pid is initialized
void getOmpShaderLocations(struct OmpShader *ompShader) {
  // Attribs
  ompShader->vertPos = glGetAttribLocation(ompShader->pid, "vertPos");
  ompShader->vertNor = glGetAttribLocation(ompShader->pid, "vertNor");

  // Per-object matrices to pass to shaders
  // Vertex shader uniforms
  ompShader->modelview = glGetUniformLocation(ompShader->pid, "modelview");
  ompShader->projection = glGetUniformLocation(ompShader->pid, "projection");
  // Fragment shader uniforms
  ompShader->materialAmbient = glGetUniformLocation(ompShader->pid,
    "material.ambient");
  ompShader->materialDiffuse = glGetUniformLocation(ompShader->pid,
    "material.diffuse");
  ompShader->materialSpecular = glGetUniformLocation(ompShader->pid,
    "material.specular");
  ompShader->materialShininess = glGetUniformLocation(ompShader->pid,
    "material.shininess");
  ompShader->lightPosition = glGetUniformLocation(ompShader->pid,
    "light.position");
  ompShader->lightAmbient = glGetUniformLocation(ompShader->pid,
    "light.ambient");
  ompShader->lightDiffuse = glGetUniformLocation(ompShader->pid,
    "light.diffuse");
  ompShader->lightSpecular = glGetUniformLocation(ompShader->pid,
    "light.specular");
}
