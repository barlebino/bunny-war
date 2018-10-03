#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "texture_shader.hpp"

// Put attrib and uniform locations into struct
// Assumes sbShader->pid is initialized
void getTextureShaderLocations(struct TextureShader *textureShader) {
  // Attribs
  textureShader->vertPos = glGetAttribLocation(textureShader->pid, "vertPos");
  textureShader->texCoord = glGetAttribLocation(textureShader->pid, "texCoord");

  // Per-object matrices to pass to shaders
  textureShader->modelview = glGetUniformLocation(textureShader->pid,
    "modelview");
  textureShader->projection = glGetUniformLocation(textureShader->pid,
    "projection");

  // Get the location of the sampler2D in fragment shader (???)
  textureShader->texLoc = glGetUniformLocation(textureShader->pid, "texCol");
}
