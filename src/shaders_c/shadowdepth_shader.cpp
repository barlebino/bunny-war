// TESTING
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shadowdepth_shader.hpp"

// Put attrib and uniform locations into struct
// Assume sdShader->pid is initialized
void getShadowDepthShaderLocations(struct ShadowDepthShader *sdShader) {
  // Attribs
  sdShader->vertPos = glGetAttribLocation(sdShader->pid,
    "vertPos");

  // Per-object matrices to pass to shaders
  sdShader->modelviewproj = glGetUniformLocation(sdShader->pid,
    "modelviewproj");
}

// Create VAO then put ID into vaoID
// Assumes shadowDepthShader locations are initilalized
void makeShadowDepthShaderVAO(
  unsigned *vaoID,
  struct ShadowDepthShader *sdShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned eleBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(sdShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(sdShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(sdShader->vertPos);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
