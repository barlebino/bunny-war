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
  GLint materialDiffuse; // samplerCube
  GLint materialSpecular; // samplerCube
  GLint materialShininess;
  GLint lightPosition;
  GLint lightAmbient;
  GLint lightDiffuse;
  GLint lightSpecular;
  // Attenuation
  GLint lightConstant;
  GLint lightLinear;
  GLint lightQuadratic;
};

// Put attrib and uniform locations into struct
void getPhongCubeShaderLocations(struct PhongCubeShader *);

// Create VAO then put ID into vaoID
// Assumes pcShader locations are initialized
void makePhongCubeShaderVAO(
  unsigned *vaoID,
  struct PhongCubeShader *pcShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID);

#endif