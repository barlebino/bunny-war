#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ofpc_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes ofpcShader->pid is initialized
void getOfpcShaderLocations(struct OfpcShader *ofpcShader) {
  // Attribs
  ofpcShader->vertPos = glGetAttribLocation(ofpcShader->pid, "vertPos");
  ofpcShader->vertNor = glGetAttribLocation(ofpcShader->pid, "vertNor");

  // Per-object matrices to pass to shaders
  ofpcShader->modelview = glGetUniformLocation(ofpcShader->pid, "modelview");
  ofpcShader->projection = glGetUniformLocation(ofpcShader->pid, "projection");
  ofpcShader->materialDiffuse = glGetUniformLocation(ofpcShader->pid,
    "material.diffuse");
  ofpcShader->materialSpecular = glGetUniformLocation(ofpcShader->pid,
    "material.specular");
  ofpcShader->materialShininess = glGetUniformLocation(ofpcShader->pid,
    "material.shininess");
  ofpcShader->lightPosition = glGetUniformLocation(ofpcShader->pid,
    "light.position");
  ofpcShader->lightAmbient = glGetUniformLocation(ofpcShader->pid,
    "light.ambient");
  ofpcShader->lightDiffuse = glGetUniformLocation(ofpcShader->pid,
    "light.diffuse");
  ofpcShader->lightSpecular = glGetUniformLocation(ofpcShader->pid,
    "light.specular");
}
