#ifndef OC_SHADER_HPP
#define OC_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct OneColorShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  // Uniforms
  GLint modelview;
  GLint projection;
  GLint in_color;
};

#endif