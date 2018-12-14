#ifndef DEFERRED_OMP_SHADER_HPP
#define DEFERRED_OMP_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct DeferredOmpShader {
  GLint pid;
  // Attribs
  GLint vertPos;
  GLint vertNor;
  // Uniforms
  // Vertex shader uniforms
  GLint modelview;
  GLint projection;
  // Fragment shader uniforms
  GLint materialDiffuse;
  GLint materialShininess;
};

// Put attrib and uniform locations into struct
void getDeferredOmpShaderLocations(
  struct DeferredOmpShader *);

// Create VAO then put ID into vaoID
// Assumes dompShader locations are initialized
void makeDeferredOmpShaderVAO(
  unsigned *vaoID,
  struct DeferredOmpShader *dompShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID,
  unsigned eleBufID);

#endif