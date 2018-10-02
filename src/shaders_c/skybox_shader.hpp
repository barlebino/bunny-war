#ifndef SKYBOX_SHADER_HPP
#define SKYBOX_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Skybox shader
struct SkyboxShader {
  GLuint pid;
  // Shader attribs
  GLint vertPos;
  // Shader uniforms
  GLint modelview;
  GLint projection;
  GLint skybox; // samplerCube
};

#endif