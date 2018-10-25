#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "phong_shader.hpp"

// Put light locations at index lightNum
// Assumes phongShader->pid is initialized
void getPhongPointLightLocations(
  struct PhongShader *phongShader, int lightNum) {
  GLint pid;
  std::string varname;
  struct PhongPointLight *ppLight;

  pid = phongShader->pid;
  ppLight = &(phongShader->pointLights[lightNum]);

  varname = "pointLights[" + std::to_string(lightNum) + "].position";
  ppLight->position = glGetUniformLocation(pid,
    varname.c_str());
  varname = "pointLights[" + std::to_string(lightNum) + "].ambient";
  ppLight->ambient = glGetUniformLocation(pid,
    varname.c_str());
  varname = "pointLights[" + std::to_string(lightNum) + "].diffuse";
  ppLight->diffuse = glGetUniformLocation(pid,
    varname.c_str());
  varname = "pointLights[" + std::to_string(lightNum) + "].specular";
  ppLight->specular = glGetUniformLocation(pid,
    varname.c_str());
  // Attenuation
  varname = "pointLights[" + std::to_string(lightNum) + "].constant";
  ppLight->constant = glGetUniformLocation(pid,
    varname.c_str());
  varname = "pointLights[" + std::to_string(lightNum) + "].linear";
  ppLight->linear = glGetUniformLocation(pid,
    varname.c_str());
  varname = "pointLights[" + std::to_string(lightNum) + "].quadratic";
  ppLight->quadratic = glGetUniformLocation(pid,
    varname.c_str());
}

// Put attrib and uniform locations into struct
// Assumes phongShader->pid is initialized
void getPhongShaderLocations(struct PhongShader *phongShader) {
  // Attribs
  phongShader->vertPos = glGetAttribLocation(phongShader->pid, "vertPos");
  phongShader->vertNor = glGetAttribLocation(phongShader->pid, "vertNor");
  phongShader->texCoord = glGetAttribLocation(phongShader->pid, "texCoord");

  // Uniforms
  phongShader->modelview = glGetUniformLocation(phongShader->pid,
    "modelview");
  phongShader->projection = glGetUniformLocation(phongShader->pid,
    "projection");
  phongShader->materialDiffuse = glGetUniformLocation(phongShader->pid,
    "material.diffuse");

  // Get all point light Uniforms
  for(int i = 0; i < NUM_POINT_LIGHTS; i++) {
    getPhongPointLightLocations(phongShader, i);
  }
}

// Create VAO then put ID into vaoID
// Assumes depthShader locations are initialized
void makePhongShaderVAO(
  unsigned *vaoID,
  struct PhongShader *phongShader,
  unsigned posBufID, // ID given by OpenGL
  unsigned norBufID,
  unsigned texCoordBufID,
  unsigned eleBufID) {
  // Create vertex array object
  glGenVertexArrays(1, vaoID);
  glBindVertexArray(*vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(phongShader->vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(phongShader->vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind normal buffer
  glEnableVertexAttribArray(phongShader->vertNor);
  glBindBuffer(GL_ARRAY_BUFFER, norBufID);
  glVertexAttribPointer(phongShader->vertNor, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind texture coordinate buffer
  glEnableVertexAttribArray(phongShader->texCoord);
  glBindBuffer(GL_ARRAY_BUFFER, texCoordBufID);
  glVertexAttribPointer(phongShader->texCoord, 2, GL_FLOAT, GL_FALSE, 
    sizeof(GL_FLOAT) * 2, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(phongShader->vertPos);
  glDisableVertexAttribArray(phongShader->vertNor);
  glDisableVertexAttribArray(phongShader->texCoord);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
