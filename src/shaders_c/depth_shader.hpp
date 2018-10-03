#ifndef DEPTH_SHADER_HPP
#define DEPTH_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// One color shader
struct DepthShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  // Uniforms
  GLint modelview;
  GLint projection;
};

// Put attrib and uniform locations into struct
void getDepthShaderLocations(struct DepthShader *);

#endif