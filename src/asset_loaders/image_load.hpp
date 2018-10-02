#ifndef IMAGE_LOAD_HPP
#define IMAGE_LOAD_HPP

#include <stdlib.h>
#include <vector>

struct Image {
  int sizeX, sizeY, numChannels;
  unsigned char *data;
};

void defaultImageLoad(const std::string &imageName, unsigned *imageID);

void defaultCubemapLoad(std::vector<std::string> faces,
  unsigned *cubemapID);

#endif