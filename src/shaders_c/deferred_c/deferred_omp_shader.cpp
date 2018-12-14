#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "deferred_omp_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes dompShader->pid is initialized
// TODO: dompShader->pid to a local variable
void getDeferredOmpShaderLocations(
  struct DeferredOmpShader *dompShader) {
  // Attribs
  dompShader->vertPos = glGetAttribLocation(dompShader->pid, "vertPos");
  dompShader->vertNor = glGetAttribLocation(dompShader->pid, "vertNor");

  // Per-object matrices to pass to shaders
  // Vertex shader uniforms
  dompShader->modelview = glGetUniformLocation(dompShader->pid, "modelview");
  dompShader->projection = glGetUniformLocation(dompShader->pid, "projection");
  // Fragment shader uniforms
  //dompShader->materialAmbient = glGetUniformLocation(dompShader->pid,
  //  "material.ambient");
  dompShader->materialDiffuse = glGetUniformLocation(dompShader->pid,
    "material.diffuse");
  //dompShader->materialSpecular = glGetUniformLocation(dompShader->pid,
  //  "material.specular");
  dompShader->materialShininess = glGetUniformLocation(dompShader->pid,
    "material.shininess");
}

// Create VAO then put ID into vaoID
// Assumes dompShader locations are initialized
void makeDeferredOmpShaderVAO(
  unsigned *vaoID,
  struct DeferredOmpShader *dompShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID,
  unsigned eleBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(dompShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(dompShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);
  
  // Bind normal buffer
  glEnableVertexAttribArray(dompShader->vertNor);
  glBindBuffer(GL_ARRAY_BUFFER, norBufID);
  glVertexAttribPointer(dompShader->vertNor, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(dompShader->vertPos);
  glDisableVertexAttribArray(dompShader->vertNor);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
