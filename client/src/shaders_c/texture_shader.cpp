#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "texture_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes sbShader->pid is initialized
void getTextureShaderLocations(struct TextureShader *textureShader) {
  // Attribs
  textureShader->vertPos = glGetAttribLocation(textureShader->pid, "vertPos");
  textureShader->texCoord = glGetAttribLocation(textureShader->pid, "texCoord");

  // Uniforms
  textureShader->exposure = glGetUniformLocation(textureShader->pid,
    "exposure");
  
  // Get the location of the sampler2D in fragment shader (???)
  textureShader->texLoc = glGetUniformLocation(textureShader->pid, "texCol");
  textureShader->bloomBuffer =
    glGetUniformLocation(textureShader->pid, "bloomBuffer");
}

// Create VAO then put ID into vaoID
// Assumes textureShader locations are initialized
void makeTextureShaderVAO(
  unsigned *vaoID,
  struct TextureShader *textureShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned texCoordBufID,
  unsigned eleBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(textureShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(textureShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind texture coordinate buffer
  glEnableVertexAttribArray(textureShader->texCoord);
  glBindBuffer(GL_ARRAY_BUFFER, texCoordBufID);
  glVertexAttribPointer(textureShader->texCoord, 2, GL_FLOAT, GL_FALSE, 
    sizeof(GL_FLOAT) * 2, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(textureShader->vertPos);
  glDisableVertexAttribArray(textureShader->texCoord);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
