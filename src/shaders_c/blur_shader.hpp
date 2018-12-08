#ifndef BLUR_SHADER_HPP
#define BLUR_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct BlurShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  GLint texCoord;
  // Uniforms
  GLint horizontal;
  GLint texLoc; // sampler2D
};

// Put attrib and uniform locations into struct
void getBlurShaderLocations(struct BlurShader *);

// Create VAO then put ID into vaoID
// Assumes blurShader locations are initialized
void makeBlurShaderVAO(
  unsigned *vaoID,
  struct BlurShader *blurShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned texCoordBufID,
  unsigned eleBufID);

#endif