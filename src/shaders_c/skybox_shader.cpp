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
