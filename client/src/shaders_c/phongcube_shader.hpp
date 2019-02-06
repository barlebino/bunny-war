#ifndef PHONGCUBE_SHADER_HPP
#define PHONGCUBE_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// TODO: Define this somewhere else
#define NUM_POINT_LIGHTS 3
#define NUM_DIRECTIONAL_LIGHTS 1

struct PhongCubeLight {
  GLint position;
  GLint ambient;
  GLint diffuse;
  GLint specular;
  // Attenuation
  GLint constant;
  GLint linear;
  GLint quadratic;
};

struct PhongCubeDirectionalLight {
  GLint direction;
  GLint ambient;
  GLint diffuse;
  GLint specular;
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
  // Shadow mapping uniforms
  GLint shadowMap;
  GLint lightspace;
  // ALL OF THE LIGHTS
  struct PhongCubeLight pointLights[NUM_POINT_LIGHTS];
  struct PhongCubeDirectionalLight
    directionalLights[NUM_DIRECTIONAL_LIGHTS];
};

// Put light locations at index lightNum into direction light struct
// Assumes pcShader->pid is initialized
void getPhongCubeDirectionalLightLocations(
  struct PhongCubeShader *pcShader, int lightNum);

// Put light locations at index lightNum into point light at lightNum
// Assumes pcShader->pid is initialized
// TODO: Change to PhongCubePointLightLocations
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