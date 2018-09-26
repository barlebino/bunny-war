#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "mesh_load.hpp"

// CPU buffers
std::vector<float> posBuf;
std::vector<float> norBuf;
std::vector<float> texCoordBuf;
std::vector<unsigned int> eleBuf;

// Puts data into posBuf, norBuf, texCoordBuf, and eleBuf
void getMesh(const std::string &meshName) {
  std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> objMaterials;
	std::string errStr;
  bool rc;

	// Clear the CPU buffers
  posBuf.clear();
  norBuf.clear();
  texCoordBuf.clear();
  eleBuf.clear();
  
  rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
	if(!rc) {
		std::cerr << errStr << std::endl;
    exit(0);
	} else {
		posBuf = shapes[0].mesh.positions;
    norBuf = shapes[0].mesh.normals;
    texCoordBuf = shapes[0].mesh.texcoords;
		eleBuf = shapes[0].mesh.indices;
	}
}

// Resizes mesh to be centered at (0, 0, 0) spanning [-1, 1]
void resizeMesh() {
  float minX, minY, minZ;
  float maxX, maxY, maxZ;
  float scaleX, scaleY, scaleZ;
  float shiftX, shiftY, shiftZ;
  float epsilon = 0.001;

  minX = minY = minZ = 1.1754E+38F;
  maxX = maxY = maxZ = -1.1754E+38F;

  //Go through all vertices to determine min and max of each dimension
  for(size_t v = 0; v < posBuf.size() / 3; v++) {
    if(posBuf[3*v+0] < minX) minX = posBuf[3*v+0];
    if(posBuf[3*v+0] > maxX) maxX = posBuf[3*v+0];

    if(posBuf[3*v+1] < minY) minY = posBuf[3*v+1];
    if(posBuf[3*v+1] > maxY) maxY = posBuf[3*v+1];

    if(posBuf[3*v+2] < minZ) minZ = posBuf[3*v+2];
    if(posBuf[3*v+2] > maxZ) maxZ = posBuf[3*v+2];
	}

	//From min and max compute necessary scale and shift for each dimension
  float maxExtent, xExtent, yExtent, zExtent;
  
  xExtent = maxX-minX;
  yExtent = maxY-minY;
  zExtent = maxZ-minZ;

  if(xExtent >= yExtent && xExtent >= zExtent) {
    maxExtent = xExtent;
  }

  if(yExtent >= xExtent && yExtent >= zExtent) {
    maxExtent = yExtent;
  }

  if(zExtent >= xExtent && zExtent >= yExtent) {
    maxExtent = zExtent;
  }

  scaleX = 2.0 / maxExtent;
  shiftX = minX + (xExtent/ 2.0);
  scaleY = 2.0 / maxExtent;
  shiftY = minY + (yExtent / 2.0);
  scaleZ = 2.0/ maxExtent;
  shiftZ = minZ + (zExtent)/2.0;

  //Go through all verticies shift and scale them
	for(size_t v = 0; v < posBuf.size() / 3; v++) {
    posBuf[3*v+0] = (posBuf[3*v+0] - shiftX) * scaleX;
    assert(posBuf[3*v+0] >= -1.0 - epsilon);
    assert(posBuf[3*v+0] <= 1.0 + epsilon);
    posBuf[3*v+1] = (posBuf[3*v+1] - shiftY) * scaleY;
    assert(posBuf[3*v+1] >= -1.0 - epsilon);
    assert(posBuf[3*v+1] <= 1.0 + epsilon);
    posBuf[3*v+2] = (posBuf[3*v+2] - shiftZ) * scaleZ;
    assert(posBuf[3*v+2] >= -1.0 - epsilon);
    assert(posBuf[3*v+2] <= 1.0 + epsilon);
  }
}

// Sends data from CPU buffers to GPU buffers
// Assumes indexed data
void sendMesh(unsigned *posBufID, unsigned *eleBufID,
  unsigned *texCoordBufID, int *eleBufSize, unsigned *norBufID) {
  // Send vertex position array to GPU
  glGenBuffers(1, posBufID);
  glBindBuffer(GL_ARRAY_BUFFER, *posBufID);
  glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0],
    GL_STATIC_DRAW);

  // Send normals array to GPU
  if(!norBuf.empty()) {
    glGenBuffers(1, norBufID);
    glBindBuffer(GL_ARRAY_BUFFER, *norBufID);
    glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float),
      &norBuf[0], GL_STATIC_DRAW);
  }

  // Send texture coordinate array to GPU
  if(!texCoordBuf.empty()) {
    glGenBuffers(1, texCoordBufID);
    glBindBuffer(GL_ARRAY_BUFFER, *texCoordBufID);
    glBufferData(GL_ARRAY_BUFFER, texCoordBuf.size() * sizeof(float),
      &texCoordBuf[0], GL_STATIC_DRAW);
  }

  // Send element array to GPU
  glGenBuffers(1, eleBufID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *eleBufID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size() * sizeof(unsigned),
    &eleBuf[0], GL_STATIC_DRAW);

  // Unbind arrays
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // This data is not on the GPU
  *eleBufSize = eleBuf.size();
}

// Skybox is not indexed, so needs separate send function
void sendSkyboxMesh(unsigned *posBufID, int *posBufSize) {
  // Send vertex position array to GPU
  glGenBuffers(1, posBufID);
  glBindBuffer(GL_ARRAY_BUFFER, *posBufID);
  glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0],
    GL_STATIC_DRAW);

  // Unbind arrays
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // This data is not on the GPU
  *posBufSize = posBuf.size();
}

// Box is not indexed, so needs separate send function
// Assumes posBufSize == norBufSize
void sendPhongBoxMesh(unsigned *posBufID, unsigned *norBufID,
  int *bufSize) {
  // Send vertex position array to GPU
  glGenBuffers(1, posBufID);
  glBindBuffer(GL_ARRAY_BUFFER, *posBufID);
  glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0],
    GL_STATIC_DRAW);

  // Send normals array to GPU
  glGenBuffers(1, norBufID);
  glBindBuffer(GL_ARRAY_BUFFER, *norBufID);
  glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float),
    &norBuf[0], GL_STATIC_DRAW);

  // This data is not on the GPU
  *bufSize = posBuf.size();
}

// "Get" operations for programmer-defined meshes
// Loads data into one or more of posBuf, norBuf, texCoordBuf, and eleBuf

void getRectangleMesh() {
  float posArr[] = {
    1.f, 1.f, 0.f,
    1.f, -1.f, 0.f,
    -1.f, 1.f, 0.f,
    -1.f, -1.f, 0.f
  };

  float texCoordArr[] = {
    1.f, 1.f,
    1.f, 0.f,
    0.f, 1.f,
    0.f, 0.f
  };

  unsigned int eleArr[] = {
    3, 0, 2,
    3, 1, 0
  };

  // Clear the CPU buffers
  // TODO: Consider changing this process
  posBuf.clear();
  norBuf.clear();
  texCoordBuf.clear();
  eleBuf.clear();

  copy(&posArr[0], &posArr[12], back_inserter(posBuf));
  copy(&texCoordArr[0], &texCoordArr[8], back_inserter(texCoordBuf));
  copy(&eleArr[0], &eleArr[6], back_inserter(eleBuf));
}

void getSkyboxMesh() {
  float posArr[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
  };

  // Clear the CPU buffers
  // TODO: Consider changing this process
  posBuf.clear();
  norBuf.clear();
  texCoordBuf.clear();
  eleBuf.clear();

  // Skybox does not need texture coordinates
  // Vertex format does not use element indexing
  // Eighteen floats per cube face
  copy(&posArr[0], &posArr[18 * 6], back_inserter(posBuf));
}

void getPhongBoxMesh() {
  /*float posArr[] = {
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,

     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,

    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
  };*/

  float posArr[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,

    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f
  };

  float norArr[] = {
     0.0f,  0.0f, -1.0f,
     0.0f,  0.0f, -1.0f,
     0.0f,  0.0f, -1.0f,
     0.0f,  0.0f, -1.0f,
     0.0f,  0.0f, -1.0f,
     0.0f,  0.0f, -1.0f,

    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,

     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,

     0.0f,  0.0f,  1.0f,
     0.0f,  0.0f,  1.0f,
     0.0f,  0.0f,  1.0f,
     0.0f,  0.0f,  1.0f,
     0.0f,  0.0f,  1.0f,
     0.0f,  0.0f,  1.0f,

     0.0f,  1.0f,  0.0f,
     0.0f,  1.0f,  0.0f,
     0.0f,  1.0f,  0.0f,
     0.0f,  1.0f,  0.0f,
     0.0f,  1.0f,  0.0f,
     0.0f,  1.0f,  0.0f,

     0.0f, -1.0f,  0.0f,
     0.0f, -1.0f,  0.0f,
     0.0f, -1.0f,  0.0f,
     0.0f, -1.0f,  0.0f,
     0.0f, -1.0f,  0.0f,
     0.0f, -1.0f,  0.0f,
  };

  // Clear the CPU buffers
  // TODO: Consider changing this process
  posBuf.clear();
  norBuf.clear();
  texCoordBuf.clear();
  eleBuf.clear();

  // Info needed for phong box
  // Does not use element indexing
  // Eighteen floats per cube face
  copy(&posArr[0], &posArr[18 * 6], back_inserter(posBuf));
  copy(&norArr[0], &norArr[18 * 6], back_inserter(norBuf));
}
