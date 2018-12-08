#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "blur_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes sbShader->pid is initialized
void getBlurShaderLocations(struct BlurShader *blurShader) {
  // Attribs
  blurShader->vertPos = glGetAttribLocation(blurShader->pid, "vertPos");
  blurShader->texCoord = glGetAttribLocation(blurShader->pid, "texCoord");

  // Uniforms
  blurShader->horizontal = glGetUniformLocation(blurShader->pid,
    "horizontal");
  // Get the location of the sampler2D in fragment shader (???)
  blurShader->texLoc = glGetUniformLocation(blurShader->pid, "texCol");
}

// Create VAO then put ID into vaoID
// Assumes blurShader locations are initialized
void makeBlurShaderVAO(
  unsigned *vaoID,
  struct BlurShader *blurShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned texCoordBufID,
  unsigned eleBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(blurShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(blurShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind texture coordinate buffer
  glEnableVertexAttribArray(blurShader->texCoord);
  glBindBuffer(GL_ARRAY_BUFFER, texCoordBufID);
  glVertexAttribPointer(blurShader->texCoord, 2, GL_FLOAT, GL_FALSE, 
    sizeof(GL_FLOAT) * 2, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(blurShader->vertPos);
  glDisableVertexAttribArray(blurShader->texCoord);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
