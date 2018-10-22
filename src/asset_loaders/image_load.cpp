#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "image_load.hpp"

// Loads regular texture data into GPU
void defaultImageLoad(const std::string &imageName, unsigned *imageID) {
  struct Image image;

  // Put data into program memory
  stbi_set_flip_vertically_on_load(true);
  image.data = stbi_load(imageName.c_str(), &(image.sizeX),
    &(image.sizeY), &(image.numChannels), 0);

  // Put data into GPU memory
  // Set 0th texture unit as active
  glActiveTexture(GL_TEXTURE0);
  // Generate texture buffer object
  glGenTextures(1, imageID);
  // Bind current texture unit to texture buffer object as a GL_TEXTURE_2D
  glBindTexture(GL_TEXTURE_2D, *imageID);
  // Load texture data into imageID
  // TODO: Change input only, but same glTexImage2D call
  if(image.numChannels == 3) {
    // Base level is 0, number of channels is 3, and border is 0
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.sizeX, image.sizeY,
      0, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte *) image.data);
  } else if(image.numChannels == 4) {
    // Base level is 0, number of channels is 4, and border is 0
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.sizeX, image.sizeY,
      0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte *) image.data);
  }

  // Generate image pyramid
  glGenerateMipmap(GL_TEXTURE_2D);
  // Set texture wrap modes for S and T directions
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // Set filtering mode for magnification and minification
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR_MIPMAP_LINEAR);

  // Unbind from texture buffer object from current texture unit
  glBindTexture(GL_TEXTURE_2D, 0);

  // Clear image
  stbi_image_free(image.data);
}

// Loads image as an sRGB texture
void gammaImageLoad(const std::string &imageName, unsigned *imageID) {
  struct Image image;

  // Put data into program memory
  stbi_set_flip_vertically_on_load(true);
  image.data = stbi_load(imageName.c_str(), &(image.sizeX),
    &(image.sizeY), &(image.numChannels), 0);

  // Put data into GPU memory
  // Set 0th texture unit as active
  glActiveTexture(GL_TEXTURE0);
  // Generate texture buffer object
  glGenTextures(1, imageID);
  // Bind current texture unit to texture buffer object as a GL_TEXTURE_2D
  glBindTexture(GL_TEXTURE_2D, *imageID);
  // Load texture data into imageID
  // TODO: Change input only, but same glTexImage2D call
  if(image.numChannels == 3) {
    // Base level is 0, number of channels is 3, and border is 0
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, image.sizeX, image.sizeY,
      0, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte *) image.data);
  } else if(image.numChannels == 4) {
    // Base level is 0, number of channels is 4, and border is 0
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, image.sizeX, image.sizeY,
      0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte *) image.data);
  }

  // Generate image pyramid
  glGenerateMipmap(GL_TEXTURE_2D);
  // Set texture wrap modes for S and T directions
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // Set filtering mode for magnification and minification
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR_MIPMAP_LINEAR);

  // Unbind from texture buffer object from current texture unit
  glBindTexture(GL_TEXTURE_2D, 0);

  // Clear image
  stbi_image_free(image.data);
}

// Loads cubemap with regular texture data
void defaultCubemapLoad(std::vector<std::string> faces,
  unsigned *cubemapID) {
  struct Image image;

  // Cubemap images are upside down
  stbi_set_flip_vertically_on_load(false);

  // Create the texture as a GL_TEXTURE_CUBE_MAP
  glGenTextures(1, cubemapID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, *cubemapID);

  for(unsigned int i = 0; i < faces.size(); i++) {
    image.data = stbi_load(faces[i].c_str(), &(image.sizeX),
      &(image.sizeY), &(image.numChannels), 0);
    if(image.data) {
      if(image.numChannels == 3) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
          0, GL_RGB, image.sizeX, image.sizeY,
          0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
        stbi_image_free(image.data);
      } else if(image.numChannels == 4) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
          0, GL_RGBA, image.sizeX, image.sizeY,
          0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
        stbi_image_free(image.data);
      }
    } else {
      std::cout << "Cubemap texture failed to load at path: " <<
        faces[i] << std::endl;
      stbi_image_free(image.data);
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // Unbind texture
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

// Loads cubemap with sRGB texture data
void gammaCubemapLoad(std::vector<std::string> faces,
  unsigned *cubemapID) {
  struct Image image;

  // Cubemap images are upside down
  stbi_set_flip_vertically_on_load(false);

  // Create the texture as a GL_TEXTURE_CUBE_MAP
  glGenTextures(1, cubemapID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, *cubemapID);

  for(unsigned int i = 0; i < faces.size(); i++) {
    image.data = stbi_load(faces[i].c_str(), &(image.sizeX),
      &(image.sizeY), &(image.numChannels), 0);
    if(image.data) {
      if(image.numChannels == 3) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
          0, GL_SRGB, image.sizeX, image.sizeY,
          0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
        stbi_image_free(image.data);
      } else if(image.numChannels == 4) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
          0, GL_SRGB_ALPHA, image.sizeX, image.sizeY,
          0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
        stbi_image_free(image.data);
      }
    } else {
      std::cout << "Cubemap texture failed to load at path: " <<
        faces[i] << std::endl;
      stbi_image_free(image.data);
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // Unbind texture
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
