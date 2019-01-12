// TESTING
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "lighting_pass_shader.hpp"

// Put light locations at index lightNum into direction light struct
// Assumes lpShader->pid is initialized
void getLightingPassDirectionalLightLocations(
  struct LightingPassShader *lpShader, int lightNum) {
  GLint pid;
  std::string varname, lightNumString;
  struct LightingPassDirectionalLight *lpdLight;

  pid = lpShader->pid;
  lpdLight = &(lpShader->directionalLights[lightNum]);
  lightNumString = std::to_string(lightNum);

  varname = "directionalLights[" + lightNumString + "].direction";
  lpdLight->direction = glGetUniformLocation(pid, varname.c_str());
  varname = "directionalLights[" + lightNumString + "].ambient";
  lpdLight->ambient = glGetUniformLocation(pid, varname.c_str());
  varname = "directionalLights[" + lightNumString + "].diffuse";
  lpdLight->diffuse = glGetUniformLocation(pid, varname.c_str());
  varname = "directionalLights[" + lightNumString + "].specular";
  lpdLight->specular = glGetUniformLocation(pid, varname.c_str());
}

// Put light locations at index lightNum
// Assumes lpShader->pid is initialized
void getLightingPassPointLightLocations(
  struct LightingPassShader *lpShader, int lightNum) {
  
}

// Put attrib and uniform locations into struct
// Assumes lpShader->pid is initialized
void getLightingPassShaderLocations(struct LightingPassShader *lpShader) {
  // Attribs
  lpShader->vertPos = glGetAttribLocation(lpShader->pid, "vertPos");
  lpShader->texCoord = glGetAttribLocation(lpShader->pid, "texCoord");

  // Uniforms
  lpShader->gpos = glGetUniformLocation(lpShader->pid, "gpos");
  lpShader->gnor = glGetUniformLocation(lpShader->pid, "gnor");
  lpShader->gcol = glGetUniformLocation(lpShader->pid, "gcol");
  lpShader->shadowMap = glGetUniformLocation(
    lpShader->pid, "shadowMap");
  lpShader->viewToLight = glGetUniformLocation(
    lpShader->pid, "viewToLight");

  // Get all directional light uniforms
  for(int i = 0; i < NUM_DIRECTIONAL_LIGHTS; i++) {
    getLightingPassDirectionalLightLocations(lpShader, i);
  }
}

// Create VAO then put ID into vaoID
// Assumes lpShader locations are initialized
void makeLightingPassShaderVAO(
  unsigned *vaoID,
  struct LightingPassShader *lpShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned texCoordBufID,
  unsigned eleBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(lpShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(lpShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind texture coordinate buffer
  glEnableVertexAttribArray(lpShader->texCoord);
  glBindBuffer(GL_ARRAY_BUFFER, texCoordBufID);
  glVertexAttribPointer(lpShader->texCoord, 2, GL_FLOAT, GL_FALSE, 
    sizeof(GL_FLOAT) * 2, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(lpShader->vertPos);
  glDisableVertexAttribArray(lpShader->texCoord);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
