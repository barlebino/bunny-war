#ifndef PHONGCUBE_SHADER_HPP
#define PHONGCUBE_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// TODO: Define this somewhere else
#define NUM_POINT_LIGHTS 3

struct PhongCubeLight {
  GLint position;
  GLint ambient;
  GLint diffuse;
  GLint specular;
  // Attentuation
  GLint constant;
  GLint linear;
  GLint quadratic;
};

struct PhongCubeShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  GLint vertNor;
  // Uniforms
  // Vertex shader uniforms
  GLint modelview;
  GLint projection;
  // Fragment shader uniforms
  GLint materialDiffuse; // samplerCube
  GLint materialSpecular; // samplerCube
  GLint materialShininess;
  // ALL OF THE LIGHTS
  struct PhongCubeLight pointLights[NUM_POINT_LIGHTS];
};

// Put light locations at index lightNum into point light at lightNum
// Assumes pcShader->pid is initialized
void getPhongCubeLightLocations(
  struct PhongCubeShader *pcShader, int lightNum);

// Put attrib and uniform locations into struct
void getPhongCubeShaderLocations(struct PhongCubeShader *);

// Create VAO then put ID into vaoID
// Assumes pcShader locations are initialized
void makePhongCubeShaderVAO(
  unsigned *vaoID,
  struct PhongCubeShader *pcShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID);

#endif