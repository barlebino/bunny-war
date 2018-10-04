#ifndef OC_SHADER_HPP
#define OC_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// One color shader
struct OneColorShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  // Uniforms
  GLint modelview;
  GLint projection;
  GLint in_color;
};

// Put attrib and uniform locations into struct
void getOneColorShaderLocations(struct OneColorShader *);

// Create VAO then put ID into vaoID
// Assumes ocShader locations are initialized
void makeOneColorShaderVAO(
  unsigned *vaoID,
  struct OneColorShader *ocShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned eleBufID);

#endif