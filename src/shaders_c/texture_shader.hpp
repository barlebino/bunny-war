#ifndef TEXTURE_SHADER_HPP
#define TEXTURE_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct TextureShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  GLint texCoord;
  // Uniforms
  GLint modelview;
  GLint projection;
  GLint texLoc; // sampler2D
};

#endif