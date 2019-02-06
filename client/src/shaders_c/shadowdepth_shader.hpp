#ifndef SHADOWDEPTH_SHADER_HPP
#define SHADOWDEPTH_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct ShadowDepthShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  // Uniforms
  GLint modelviewproj;
};

// Put attrib and uniform locations into struct
void getShadowDepthShaderLocations(struct ShadowDepthShader *);

// Create VAO then put ID into vaoID
// Assumes shadowDepthShader locations are initilalized
void makeShadowDepthShaderVAO(
  unsigned *vaoID,
  struct ShadowDepthShader *shadowDepthShader,
  unsigned posBufID,
  unsigned eleBufID);

#endif