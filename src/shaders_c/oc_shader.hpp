#ifndef OC_SHADER_HPP
#define OC_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// One color shader
struct OcShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  // Uniforms
  GLint modelview;
  GLint projection;
  GLint in_color;
};

// Put attrib and uniform locations into struct
void getOcShaderLocations(struct OcShader *);

#endif