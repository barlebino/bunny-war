#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "onefacephongcube_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes ofpcShader->pid is initialized
void getOneFacePhongCubeShaderLocations(
  struct OneFacePhongCubeShader *ofpcShader) {
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
  // Attenuation
  ofpcShader->lightConstant = glGetUniformLocation(ofpcShader->pid,
    "light.constant");
  ofpcShader->lightLinear = glGetUniformLocation(ofpcShader->pid,
    "light.linear");
  ofpcShader->lightQuadratic = glGetUniformLocation(ofpcShader->pid,
    "light.quadratic");
}

// Create VAO then put ID into vaoID
// Assumes ofpcShader locations are initialized
void makeOneFacePhongCubeShaderVAO(
  unsigned *vaoID,
  struct OneFacePhongCubeShader *ofpcShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(ofpcShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(ofpcShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind normal buffer
  glEnableVertexAttribArray(ofpcShader->vertNor);
  glBindBuffer(GL_ARRAY_BUFFER, norBufID);
  glVertexAttribPointer(ofpcShader->vertNor, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);
  
  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(ofpcShader->vertPos);
  glDisableVertexAttribArray(ofpcShader->vertNor);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
