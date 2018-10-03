#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "oc_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes ocShader->pid is initialized
void getOcShaderLocations(struct OcShader *ocShader) {
  // Attribs
  ocShader->vertPos = glGetAttribLocation(ocShader->pid, "vertPos");

  // Per-object matrices to pass to shaders
  ocShader->modelview = glGetUniformLocation(ocShader->pid, "modelview");
  ocShader->projection = glGetUniformLocation(ocShader->pid, "projection");
  ocShader->in_color = glGetUniformLocation(ocShader->pid, "in_color");
}
