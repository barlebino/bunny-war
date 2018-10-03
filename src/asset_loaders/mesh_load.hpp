#ifndef MESH_LOAD_HPP
#define MESH_LOAD_HPP

#include <stdlib.h>
#include <vector>

// Puts data into posBuf, norBuf, texCoordBuf, and eleBuf
void getMesh(const std::string &meshName);

// Resizes mesh to be centered at (0, 0, 0) spanning [-1, 1]
void resizeMesh();

// Sends data from CPU buffers to GPU buffers
// Assumes indexed data
// Places IDs into the memory locations specified by the parameters
void sendMesh(unsigned *posBufID, unsigned *eleBufID,
  unsigned *texCoordBufID, int *eleBufSize, unsigned *norBufID);

// Skybox is not indexed, so needs separate send function
void sendSkyboxMesh(unsigned *posBufID, int *posBufSize);

// Box is not indexed, so needs separate send function
// Assumes posBufSize == norBufSize
void sendPhongBoxMesh(unsigned *posBufID, unsigned *norBufID,
  int *bufSize);

// "Get" operations for programmer-defined meshes
// Loads data into one or more of posBuf, norBuf, texCoordBuf, and eleBuf
void getRectangleMesh();
void getSkyboxMesh();
void getPhongBoxMesh();

#endif