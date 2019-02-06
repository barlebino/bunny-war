#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "depth_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes depthShader->pid is initialized
void getDepthShaderLocations(struct DepthShader *depthShader) {
  // Attribs
  depthShader->vertPos = glGetAttribLocation(depthShader->pid, "vertPos");

  // Per-object matrices to pass to shaders
  depthShader->modelview = glGetUniformLocation(depthShader->pid, "modelview");
  depthShader->projection = glGetUniformLocation(depthShader->pid,
    "projection");
}

// Create VAO then put ID into vaoID
// Assumes depthShader locations are initialized
void makeDepthShaderVAO(
  unsigned *vaoID,
  struct DepthShader *depthShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned eleBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(depthShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(depthShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(depthShader->vertPos);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
