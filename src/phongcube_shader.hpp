#ifndef PHONGCUBE_SHADER_HPP
#define PHONGCUBE_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct PhongCubeShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  GLint vertNor;
  // Uniforms
  // Vertex shader uniforms
  GLint modelview;
  GLint projection;
  // Fragment shader uniforms
  GLint camPos;
  GLint materialDiffuse; // samplerCube
  GLint materialSpecular; // samplerCube
  GLint materialShininess;
  GLint lightPosition;
  GLint lightAmbient;
  GLint lightDiffuse;
  GLint lightSpecular;
};

#endif