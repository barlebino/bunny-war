#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "onematerialphong_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes ompShader->pid is initialized
void getOneMaterialPhongShaderLocations(
  struct OneMaterialPhongShader *ompShader) {
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
  // Attenuation
  ompShader->lightConstant = glGetUniformLocation(ompShader->pid,
    "light.constant");
  ompShader->lightLinear = glGetUniformLocation(ompShader->pid,
    "light.linear");
  ompShader->lightQuadratic = glGetUniformLocation(ompShader->pid,
    "light.quadratic");
}

// Create VAO then put ID into vaoID
// Assumes ompShader locations are initialized
void makeOneMaterialPhongShaderVAO(
  unsigned *vaoID,
  struct OneMaterialPhongShader *ompShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID,
  unsigned eleBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(ompShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(ompShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);
  
  // Bind normal buffer
  glEnableVertexAttribArray(ompShader->vertNor);
  glBindBuffer(GL_ARRAY_BUFFER, norBufID);
  glVertexAttribPointer(ompShader->vertNor, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(ompShader->vertPos);
  glDisableVertexAttribArray(ompShader->vertNor);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
