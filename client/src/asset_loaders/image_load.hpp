#ifndef IMAGE_LOAD_HPP
#define IMAGE_LOAD_HPP

#include <stdlib.h>
#include <vector>

struct Image {
  int sizeX, sizeY, numChannels;
  unsigned char *data;
};

// Loads regular texture data into GPU
void defaultImageLoad(const std::string &imageName, unsigned *imageID);

// Loads image as an sRGB texture
void gammaImageLoad(const std::string &imageName, unsigned *imageID);

// Loads cubemap with regular texture data
void defaultCubemapLoad(std::vector<std::string> faces,
  unsigned *cubemapID);

// Loads cubemap with sRGB texture data
void gammaCubemapLoad(std::vector<std::string> faces,
  unsigned *cubemapID);

#endif