#ifndef LIGHTING_PASS_SHADER_HPP
#define LIGHTING_PASS_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct LightingPassShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  GLint texCoord;
  // Uniforms
  GLint gpos;
  GLint gnor;
  GLint gcol;
};

// Put attrib and uniform locations into struct
void getLightingPassShaderLocations(struct LightingPassShader *);

// Create VAO then put ID into vaoID
// Assumes lpShader locations are initialized
void makeLightingPassShaderVAO(
  unsigned *vaoID,
  struct LightingPassShader *lpShader,
  unsigned posBufID,
  unsigned texCoordBufID,
  unsigned eleBufID);

#endif