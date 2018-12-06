#ifndef TEXTURE_SHADER_HPP
#define TEXTURE_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct TextureShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  GLint texCoord;
  // Uniforms
  GLint exposure;
  GLint texLoc; // sampler2D
};

// Put attrib and uniform locations into struct
void getTextureShaderLocations(struct TextureShader *);

// Create VAO then put ID into vaoID
// Assumes textureShader locations are initialized
void makeTextureShaderVAO(
  unsigned *vaoID,
  struct TextureShader *textureShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned texCoordBufID,
  unsigned eleBufID);

#endif