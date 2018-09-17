#ifndef SB_SHADER_HPP
#define SB_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Skybox shader
struct SbShader {
  GLuint pid;
  // Shader attribs
  GLint vertPos;
  // Shader uniforms
  GLint modelview;
  GLint projection;
  GLint texLoc; // samplerCube
};

#endif