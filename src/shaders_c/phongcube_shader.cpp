#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "phongcube_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes pcShader->pid is initialized
void getPhongCubeShaderLocations(struct PhongCubeShader *pcShader) {
  // Attribs
  pcShader->vertPos = glGetAttribLocation(pcShader->pid, "vertPos");
  pcShader->vertNor = glGetAttribLocation(pcShader->pid, "vertNor");

  // Per-object matrices to pass to shaders
  // Vertex shader uniforms
  pcShader->modelview = glGetUniformLocation(pcShader->pid, "modelview");
  pcShader->projection = glGetUniformLocation(pcShader->pid, "projection");
  // Fragment shader uniforms
  pcShader->materialDiffuse = glGetUniformLocation(pcShader->pid,
    "material.diffuse");
  pcShader->materialSpecular = glGetUniformLocation(pcShader->pid,
    "material.specular");
  pcShader->materialShininess = glGetUniformLocation(pcShader->pid,
    "material.shininess");
  pcShader->lightPosition = glGetUniformLocation(pcShader->pid,
    "light.position");
  pcShader->lightAmbient = glGetUniformLocation(pcShader->pid,
    "light.ambient");
  pcShader->lightDiffuse = glGetUniformLocation(pcShader->pid,
    "light.diffuse");
  pcShader->lightSpecular = glGetUniformLocation(pcShader->pid,
    "light.specular");
}
