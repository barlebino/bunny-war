#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "onecolor_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes ocShader->pid is initialized
void getOneColorShaderLocations(struct OneColorShader *ocShader) {
  // Attribs
  ocShader->vertPos = glGetAttribLocation(ocShader->pid, "vertPos");

  // Per-object matrices to pass to shaders
  ocShader->modelview = glGetUniformLocation(ocShader->pid, "modelview");
  ocShader->projection = glGetUniformLocation(ocShader->pid, "projection");
  ocShader->in_color = glGetUniformLocation(ocShader->pid, "in_color");
}

// Create VAO then put ID into vaoID
// Assumes ocShader locations are initialized
void makeOneColorShaderVAO(
  unsigned *vaoID,
  struct OneColorShader *ocShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned eleBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(ocShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(ocShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(ocShader->vertPos);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
