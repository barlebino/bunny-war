#ifndef ONEFACEPHONGCUBE_SHADER_HPP
#define ONEFACEPHONGCUBE_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// One face phong cube
struct OneFacePhongCubeShader {
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
  // Attenuation
  GLint lightConstant;
  GLint lightLinear;
  GLint lightQuadratic;
};

// Put attrib and uniform locations into struct
void getOneFacePhongCubeShaderLocations(
  struct OneFacePhongCubeShader *);

// Create VAO then put ID into vaoID
// Assumes ofpcShader locations are initialized
void makeOneFacePhongCubeShaderVAO(
  unsigned *vaoID,
  struct OneFacePhongCubeShader *ofpcShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID);

#endif