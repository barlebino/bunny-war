#ifndef ONEMATERIALPHONG_SHADER_HPP
#define ONEMATERIALPHONG_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NUM_POINT_LIGHTS 3

struct OneMaterialPhongPointLight {
  GLint position;
  GLint ambient;
  GLint diffuse;
  GLint specular;
  // Attentuation
  GLint constant;
  GLint linear;
  GLint quadratic;
};

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
  // ALL OF THE LIGHTS
  struct OneMaterialPhongPointLight ompLights[NUM_POINT_LIGHTS];
};

// Put light locations at index lightNum into struct
void getOneMaterialPhongPointLightLocations(GLint pid,
  struct OneMaterialPhongPointLight *ompLight, int lightNum);

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