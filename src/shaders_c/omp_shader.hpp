#ifndef OMP_SHADER_HPP
#define OMP_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct OmpShader {
  GLint pid;
  // Attribs
  GLint vertPos;
  GLint vertNor;
  // Uniforms
  // Vertex shader uniforms
  GLint modelview;
  GLint projection;
  // Fragment shader uniforms
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