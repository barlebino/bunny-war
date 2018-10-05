#ifndef PHONG_SHADER_HPP
#define PHONG_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// One color shader
struct PhongShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  GLint vertNor;
  GLint texCoord;
  // Uniforms
  GLint modelview;
  GLint projection;
  GLint materialDiffuse;
  GLint lightPosition;
  GLint lightAmbient;
  GLint lightDiffuse;
  GLint lightSpecular;
  GLint lightConstant;
  GLint lightLinear;
  GLint lightQuadratic;
};

// Put attrib and uniform locations into struct
void getPhongShaderLocations(struct PhongShader *);

// Create VAO then put ID into vaoID
// Assumes phongShader locations are initialized
void makePhongShaderVAO(
  unsigned *vaoID,
  struct PhongShader *phongShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID,
  unsigned texCoordBufID,
  unsigned eleBufID);

#endif