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
