#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "skybox_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes sbShader->pid is initialized
void getSkyboxShaderLocations(struct SkyboxShader *sbShader) {
  // Attribs
  sbShader->vertPos = glGetAttribLocation(sbShader->pid, "vertPos");

  // Per-object matrices to pass to shaders
  sbShader->modelview = glGetUniformLocation(sbShader->pid, "modelview");
  sbShader->projection = glGetUniformLocation(sbShader->pid, "projection");

  // Get the location of the samplerCube in fragment shader (???)
  sbShader->skybox = glGetUniformLocation(sbShader->pid, "skybox");
}

// Create VAO then put ID into vaoID
// Assumes skyboxShader locations are initialized
void makeSkyboxShaderVAO(
  unsigned *vaoID,
  struct SkyboxShader *skyboxShader,
  unsigned posBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(skyboxShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(skyboxShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(skyboxShader->vertPos);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
