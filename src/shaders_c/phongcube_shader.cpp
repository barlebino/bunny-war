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
  // Attenuation
  pcShader->lightConstant = glGetUniformLocation(pcShader->pid,
    "light.constant");
  pcShader->lightLinear = glGetUniformLocation(pcShader->pid,
    "light.linear");
  pcShader->lightQuadratic = glGetUniformLocation(pcShader->pid,
    "light.quadratic");
}

// Create VAO then put ID into vaoID
// Assumes pcShader locations are initialized
void makePhongCubeShaderVAO(
  unsigned *vaoID,
  struct PhongCubeShader *pcShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(pcShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(pcShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind normal buffer
  glEnableVertexAttribArray(pcShader->vertNor);
  glBindBuffer(GL_ARRAY_BUFFER, norBufID);
  glVertexAttribPointer(pcShader->vertNor, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(pcShader->vertPos);
  glDisableVertexAttribArray(pcShader->vertNor);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
