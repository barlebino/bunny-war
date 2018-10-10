#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "phongcube_shader.hpp"

// Put light locations at index lightNum into point light at lightNum
// Assumes pcShader->pid is initialized
void getPhongCubeLightLocations(
  struct PhongCubeShader *pcShader, int lightNum) {
  GLint pid;
  std::string varname, lightNumString;
  struct PhongCubeLight *pcLight;

  pid = pcShader->pid;
  pcLight = &(pcShader->pointLights[lightNum]);
  lightNumString = std::to_string(lightNum);

  varname = "pointLights[" + lightNumString + "].position";
  pcLight->position = glGetUniformLocation(pid,
    varname.c_str());
  varname = "pointLights[" + lightNumString + "].ambient";
  pcLight->ambient = glGetUniformLocation(pid,
    varname.c_str());
  varname = "pointLights[" + lightNumString + "].diffuse";
  pcLight->diffuse = glGetUniformLocation(pid,
    varname.c_str());
  varname = "pointLights[" + lightNumString + "].specular";
  pcLight->specular = glGetUniformLocation(pid,
    varname.c_str());
  // Attenuation
  varname = "pointLights[" + lightNumString + "].constant";
  pcLight->constant = glGetUniformLocation(pid,
    varname.c_str());
  varname = "pointLights[" + lightNumString + "].linear";
  pcLight->linear = glGetUniformLocation(pid,
    varname.c_str());
  varname = "pointLights[" + lightNumString + "].quadratic";
  pcLight->quadratic = glGetUniformLocation(pid,
    varname.c_str());
}

// Put attrib and uniform locations into struct
// Assumes pcShader->pid is initialized
void getPhongCubeShaderLocations(struct PhongCubeShader *pcShader) {
  // Attribs
  pcShader->vertPos = glGetAttribLocation(pcShader->pid, "vertPos");
  pcShader->vertNor = glGetAttribLocation(pcShader->pid, "vertNor");

  // Per-object matrices to pass to shaders
  // Vertex shader uniforms
  pcShader->modelview = glGetUniformLocation(pcShader->pid, "modelview");
  pcShader->projection = glGetUniformLocation(pcShader->pid, "projection");
  // Fragment shader uniforms
  pcShader->materialDiffuse = glGetUniformLocation(pcShader->pid,
    "material.diffuse");
  pcShader->materialSpecular = glGetUniformLocation(pcShader->pid,
    "material.specular");
  pcShader->materialShininess = glGetUniformLocation(pcShader->pid,
    "material.shininess");
  
  // Get all point light uniforms
  for(int i = 0; i < NUM_POINT_LIGHTS; i++) {
    getPhongCubeLightLocations(pcShader, i);
  }
}

// Create VAO then put ID into vaoID
// Assumes pcShader locations are initialized
void makePhongCubeShaderVAO(
  unsigned *vaoID,
  struct PhongCubeShader *pcShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(pcShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(pcShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind normal buffer
  glEnableVertexAttribArray(pcShader->vertNor);
  glBindBuffer(GL_ARRAY_BUFFER, norBufID);
  glVertexAttribPointer(pcShader->vertNor, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(pcShader->vertPos);
  glDisableVertexAttribArray(pcShader->vertNor);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
