#ifndef ONEFACEPHONGCUBE_SHADER_HPP
#define ONEFACEPHONGCUBE_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// TODO: Define this somewhere else
#define NUM_POINT_LIGHTS 3
#define NUM_DIRECTIONAL_LIGHTS 1

// TODO: Change to "DirectionalLight"
struct OneFacePhongCubeLight {
  GLint position;
  GLint ambient;
  GLint diffuse;
  GLint specular;
  // Attenuation
  GLint constant;
  GLint linear;
  GLint quadratic;
};

struct OneFacePhongCubeDirectionalLight {
  GLint direction;
  GLint ambient;
  GLint diffuse;
  GLint specular;
};

// One face phong cube
struct OneFacePhongCubeShader {
  GLint pid;
  // Attribs
  GLint vertPos;
  GLint vertNor;
  // Uniforms
  // Vertex shader uniforms
  GLint modelview;
  GLint projection;
  // Fragment shader uniforms
  GLint materialDiffuse; // sampler2D
  GLint materialSpecular; // sampler2D
  GLint materialShininess;
  // Shadow mapping uniforms
  GLint shadowMap;
  GLint lightspace;
  // ALL OF THE LIGHTS
  struct OneFacePhongCubeLight pointLights[NUM_POINT_LIGHTS];
  struct OneFacePhongCubeDirectionalLight
    directionalLights[NUM_DIRECTIONAL_LIGHTS];
};

// Put light locations at index lightNum into direction light struct
// Assumes ofpcShader->pid is initialized
void getOneFacePhongCubeDirectionalLightLocations(
  struct OneFacePhongCubeShader *ofpcShader, int lightNum);

// Put light locations at index lightNum into point light at lightNum
// Assumes ofpcShader->pid is initialized
void getOneFacePhongCubeLightLocations(
  struct OneFacePhongCubeShader *ofpcShader, int lightNum);

// Put attrib and uniform locations into struct
void getOneFacePhongCubeShaderLocations(
  struct OneFacePhongCubeShader *);

// Create VAO then put ID into vaoID
// Assumes ofpcShader locations are initialized
void makeOneFacePhongCubeShaderVAO(
  unsigned *vaoID,
  struct OneFacePhongCubeShader *ofpcShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID);

#endif