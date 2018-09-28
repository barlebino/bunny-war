#ifndef OFPC_SHADER_HPP
#define OFPC_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct OfpcShader {
  GLint pid;
  // Attribs
  GLint vertPos;
  GLint vertNor;
  // Uniforms
  // Vertex shader uniforms
  GLint modelview;
  GLint projection;
  // Fragment shader uniforms
  GLint materialDiffuse; // sampler2D
  GLint materialSpecular; // sampler2D
  GLint materialShininess;
  GLint lightPosition;
  GLint lightAmbient;
  GLint lightDiffuse;
  GLint lightSpecular;
};

#endif