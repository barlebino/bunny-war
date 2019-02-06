#ifndef RECT_SHADER_HPP
#define RECT_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Skybox shader
struct RectShader {
  GLuint pid;
  // Shader attribs
  GLint vertPos;
  GLint texCoord;
  // Shader uniforms
  GLint texLoc; // samplerCube
};

// Put attrib and uniform locations into struct
void getRectShaderLocations(struct RectShader *);

// Create VAO then put ID into vaoID
// Assumes rectShader locations are initialized
void makeRectShaderVAO(
  unsigned *vaoID,
  struct RectShader *rectShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned texCoordBufID,
  unsigned eleBufID);

#endif