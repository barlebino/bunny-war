#ifndef SKYBOX_SHADER_HPP
#define SKYBOX_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Skybox shader
struct SkyboxShader {
  GLuint pid;
  // Shader attribs
  GLint vertPos;
  // Shader uniforms
  GLint modelview;
  GLint projection;
  GLint skybox; // samplerCube
};

// Put attrib and uniform locations into struct
void getSkyboxShaderLocations(struct SkyboxShader *);

// Create VAO then put ID into vaoID
// Assumes skyboxShader locations are initialized
void makeSkyboxShaderVAO(
  unsigned *vaoID,
  struct SkyboxShader *skyboxShader,
  unsigned posBufID);

#endif