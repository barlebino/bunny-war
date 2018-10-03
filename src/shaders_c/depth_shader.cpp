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
  depthShader->projection = glGetUniformLocation(depthShader->pid, "projection");
}
