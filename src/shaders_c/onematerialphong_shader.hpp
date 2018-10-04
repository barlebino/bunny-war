#ifndef ONEMATERIALPHONG_SHADER_HPP
#define ONEMATERIALPHONG_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct OneMaterialPhongShader {
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
  // Attentuation
  GLint lightConstant;
  GLint lightLinear;
  GLint lightQuadratic;
};

// Put attrib and uniform locations into struct
void getOneMaterialPhongShaderLocations(
  struct OneMaterialPhongShader *);

// Create VAO then put ID into vaoID
// Assumes ompShader locations are initialized
void makeOneMaterialPhongShaderVAO(
  unsigned *vaoID,
  struct OneMaterialPhongShader *ompShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID,
  unsigned eleBufID);

#endif