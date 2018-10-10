#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "onefacephongcube_shader.hpp"

void getOneFacePhongCubeLightLocations(
  struct OneFacePhongCubeShader *ofpcShader, int lightNum) {
  GLint pid;
  std::string varname, lightNumString;
  struct OneFacePhongCubeLight *ofpcLight;

  pid = ofpcShader->pid;
  ofpcLight = &(ofpcShader->pointLights[lightNum]);
  lightNumString = std::to_string(lightNum);

  varname = "pointLights[" + lightNumString + "].position";
  ofpcLight->position = glGetUniformLocation(pid, varname.c_str());
  varname = "pointLights[" + lightNumString + "].ambient";
  ofpcLight->ambient = glGetUniformLocation(pid, varname.c_str());
  varname = "pointLights[" + lightNumString + "].diffuse";
  ofpcLight->diffuse = glGetUniformLocation(pid, varname.c_str());
  varname = "pointLights[" + lightNumString + "].specular";
  ofpcLight->specular = glGetUniformLocation(pid, varname.c_str());
  // Attenuation
  varname = "pointLights[" + lightNumString + "].constant";
  ofpcLight->constant = glGetUniformLocation(pid, varname.c_str());
  varname = "pointLights[" + lightNumString + "].linear";
  ofpcLight->linear = glGetUniformLocation(pid, varname.c_str());
  varname = "pointLights[" + lightNumString + "].quadratic";
  ofpcLight->quadratic = glGetUniformLocation(pid, varname.c_str());
}

// Put attrib and uniform locations into struct
// Assumes ofpcShader->pid is initialized
void getOneFacePhongCubeShaderLocations(
  struct OneFacePhongCubeShader *ofpcShader) {
  // Attribs
  ofpcShader->vertPos = glGetAttribLocation(ofpcShader->pid, "vertPos");
  ofpcShader->vertNor = glGetAttribLocation(ofpcShader->pid, "vertNor");

  // Per-object matrices to pass to shaders
  ofpcShader->modelview = glGetUniformLocation(ofpcShader->pid, "modelview");
  ofpcShader->projection = glGetUniformLocation(ofpcShader->pid, "projection");
  ofpcShader->materialDiffuse = glGetUniformLocation(ofpcShader->pid,
    "material.diffuse");
  ofpcShader->materialSpecular = glGetUniformLocation(ofpcShader->pid,
    "material.specular");
  ofpcShader->materialShininess = glGetUniformLocation(ofpcShader->pid,
    "material.shininess");
  
  // Get all point light uniforms
  for(int i = 0; i < NUM_POINT_LIGHTS; i++) {
    getOneFacePhongCubeLightLocations(ofpcShader, i);
  }
}

// Create VAO then put ID into vaoID
// Assumes ofpcShader locations are initialized
void makeOneFacePhongCubeShaderVAO(
  unsigned *vaoID,
  struct OneFacePhongCubeShader *ofpcShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(ofpcShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(ofpcShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind normal buffer
  glEnableVertexAttribArray(ofpcShader->vertNor);
  glBindBuffer(GL_ARRAY_BUFFER, norBufID);
  glVertexAttribPointer(ofpcShader->vertNor, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);
  
  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(ofpcShader->vertPos);
  glDisableVertexAttribArray(ofpcShader->vertNor);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
