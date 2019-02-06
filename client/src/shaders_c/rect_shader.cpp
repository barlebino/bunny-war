#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "rect_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes rectShader->pid is initialized
void getRectShaderLocations(struct RectShader *rectShader) {
  // Attribs
  rectShader->vertPos = glGetAttribLocation(rectShader->pid, "vertPos");
  rectShader->texCoord = glGetAttribLocation(rectShader->pid, "texCoord");

  // Get the location of the sampler2D in fragment shader (???)
  rectShader->texLoc = glGetUniformLocation(rectShader->pid, "texCol");
}

// Create VAO then put ID into vaoID
// Assumes rectShader locations are initialized
void makeRectShaderVAO(
  unsigned *vaoID,
  struct RectShader *rectShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned texCoordBufID,
  unsigned eleBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);
  
  // Bind position buffer
  glEnableVertexAttribArray(rectShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(rectShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind texture coordinate buffer
  glEnableVertexAttribArray(rectShader->texCoord);
  glBindBuffer(GL_ARRAY_BUFFER, texCoordBufID);
  glVertexAttribPointer(rectShader->texCoord, 2, GL_FLOAT, GL_FALSE, 
    sizeof(GL_FLOAT) * 2, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(rectShader->vertPos);
  glDisableVertexAttribArray(rectShader->texCoord);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
