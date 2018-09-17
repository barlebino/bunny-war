#ifndef RECT_SHADER_HPP
#define RECT_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Skybox shader
struct RectShader {
  GLuint pid;
  // Shader attribs
  GLint vertPos;
  GLint texCoord;
  // Shader uniforms
  GLint texLoc; // samplerCube
};

#endif