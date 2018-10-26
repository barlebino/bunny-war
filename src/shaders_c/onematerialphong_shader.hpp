#ifndef ONEMATERIALPHONG_SHADER_HPP
#define ONEMATERIALPHONG_SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// TODO: Define this somewhere else
#define NUM_POINT_LIGHTS 3
#define NUM_DIRECTIONAL_LIGHTS 1

struct OneMaterialPhongPointLight {
  GLint position;
  GLint ambient;
  GLint diffuse;
  GLint specular;
  // Attentuation
  GLint constant;
  GLint linear;
  GLint quadratic;
};

struct OneMaterialPhongDirectionalLight {
  GLint direction;
  GLint ambient;
  GLint diffuse;
  GLint specular;
};

struct OneMaterialPhongShader {
  GLint pid;
  // Attribs
  GLint vertPos;
  GLint vertNor;
  // Uniforms
  // Vertex shader uniforms
  GLint modelview;
  GLint projection;
  // Fragment shader uniforms
  GLint materialAmbient;
  GLint materialDiffuse;
  GLint materialSpecular;
  GLint materialShininess;
  // ALL OF THE LIGHTS
  // TODO: change ompLights to pointLights
  struct OneMaterialPhongPointLight pointLights[NUM_POINT_LIGHTS];
  struct OneMaterialPhongDirectionalLight
    directionalLights[NUM_DIRECTIONAL_LIGHTS];
};

// Put light locations at index lightNum into direction light struct
// Assumes ompShader->pid is initialized
void getOneMaterialPhongDirectionalLightLocations(
  struct OneMaterialPhongShader *ompShader, int lightNum);

// Put light locations at index lightNum into pointLight struct at lightNum
// Assumes ompShader->pid is initialized
void getOneMaterialPhongPointLightLocations(
  struct OneMaterialPhongShader *ompShader, int lightNum);

// Put attrib and uniform locations into struct
void getOneMaterialPhongShaderLocations(
  struct OneMaterialPhongShader *);

// Create VAO then put ID into vaoID
// Assumes ompShader locations are initialized
void makeOneMaterialPhongShaderVAO(
  unsigned *vaoID,
  struct OneMaterialPhongShader *ompShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID,
  unsigned eleBufID);

#endif