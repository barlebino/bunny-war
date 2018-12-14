#ifndef LIGHTING_PASS_SHADER_HPP
#define LIGHTING_PASS_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// TODO: Define this somewhere else
#define NUM_POINT_LIGHTS 3
#define NUM_DIRECTIONAL_LIGHTS 1

// TODO: Point light same for all shaders?
struct LightingPassPointLight {
  GLint position;
  GLint ambient;
  GLint diffuse;
  GLint specular;
  // Attenuation
  GLint constant;
  GLint linear;
  GLint quadratic;
};

struct LightingPassDirectionalLight {
  GLint direction;
  GLint ambient;
  GLint diffuse;
  GLint specular;
};

struct LightingPassShader {
  GLuint pid;
  // Attribs
  GLint vertPos;
  GLint texCoord;
  // Uniforms
  GLint gpos;
  GLint gnor;
  GLint gcol;
  // TODO: Shadow mapping uniforms
  // ALL OF THE LIGHTS
  struct LightingPassPointLight pointLights[NUM_POINT_LIGHTS];
  struct LightingPassDirectionalLight
    directionalLights[NUM_DIRECTIONAL_LIGHTS];
};

// Put light locations at index lightNum into direction light struct
// Assumes lpShader->pid is initialized
void getLightingPassDirectionalLightLocations(
  struct LightingPassShader *lpShader, int lightNum);

// Put light locations at index lightNum
// Assumes lpShader->pid is initialized
void getLightingPassPointLightLocations(
  struct LightingPassShader *lpShader, int lightNum);

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