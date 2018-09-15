#ifndef OMP_SHADER_HPP
#define OMP_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct OmpShader {
  GLint pid;
  // Shader attribs
  GLint vertPos;
  GLint vertNor;
  // Shader uniforms
  // Vertex shader uniforms
  GLint model;
  GLint view;
  GLint projection;
  // Fragment shader uniforms
  GLint camPos;
  GLint materialAmbient;
  GLint materialDiffuse;
  GLint materialSpecular;
  GLint materialShininess;
  GLint lightPosition;
  GLint lightAmbient;
  GLint lightDiffuse;
  GLint lightSpecular;
};

#endif