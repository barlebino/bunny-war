#include <iostream>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.14159

#include <unistd.h>

#include "material.hpp"
#include "omp_shader.hpp"
#include "oc_shader.hpp"
#include "sb_shader.hpp"

// Image code, for textures
struct Image {
  int sizeX, sizeY, numChannels;
  unsigned char *data;
};

// Light struct
struct Light {
  glm::vec3 position;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
};

int ssaaLevel = 2;

GLFWwindow *window; // Main application window

// Location of camera
glm::vec3 camLocation = glm::vec3(0.f, 0.f, 2.f);
// Rotation of camera
glm::vec2 camRotation = glm::vec2(0.f, 0.f);
// Forward direction
glm::vec3 forward = glm::vec3(0.f, 0.f, -1.f);
// Sideways direction
glm::vec3 sideways = glm::vec3(1.f, 0.f, 0.f);

// Light object
Light tutorialLight = {
  glm::vec3(-8.f, 0.f, -2.f),
  glm::vec3(.2f, .2f, .2f),
  glm::vec3(.5f, .5f, .5f),
  glm::vec3(1.f, 1.f, 1.f)
};

// Input
char keys[6] = {0, 0, 0, 0, 0, 0};

// CPU buffers
std::vector<float> posBuf;
std::vector<float> norBuf;
std::vector<float> texCoordBuf;
std::vector<unsigned int> eleBuf;

// VAO IDs
unsigned to_vaoID;
unsigned do_vaoID;
unsigned do_sphere_vaoID;
unsigned rect_vaoID;
unsigned grass_vaoID;
unsigned skybox_vaoID;
unsigned oc_bunny_vaoID;
unsigned ls_vaoID;
unsigned omp_bunny_vaoID;

// Sphere data
unsigned sphere_posBufID;
unsigned sphere_norBufID;
unsigned sphere_eleBufID;
unsigned sphere_texCoordBufID;
unsigned sphere_texBufID;
int sphere_eleBufSize;

// Bunny data
unsigned bunny_posBufID;
unsigned bunny_norBufID;
unsigned bunny_eleBufID;
int bunny_eleBufSize;

// Rectangle data
unsigned rect_posBufID;
unsigned rect_eleBufID;
unsigned rect_texCoordBufID;
unsigned rect_texBufID;
int rect_eleBufSize;

// Grass data (uses rectangle)
unsigned grass_texBufID;

// Skybox data
unsigned skybox_posBufID;
unsigned skybox_posBufSize;

// Shader programs

// Texture only
GLuint to_pid;
// Shader attribs
GLint to_vertPosLoc;
GLint to_texCoordLoc;
// Shader uniforms
GLint to_modelviewLoc;
GLint to_projectionLoc;
// sampler2D location
GLint to_texLoc;

// Depth only
GLuint do_pid;
// Shader attribs
GLint do_vertPosLoc;
// Shader uniforms
GLint do_modelviewLoc;
GLint do_projectionLoc;

// Rectangle shader
GLuint r_pid;
// Shader attribs
GLint r_vertPosLoc;
GLint r_texCoordLoc;
// Shader uniforms
// None
// sampler2D location
GLint r_texLoc;

// TODO: Convert to enums ???
// TODO: Change to omp (one material phong) shader
// TODO: Create cube phong
// Phong shader
GLuint phong_pid;
// Shader attribs
GLint phong_vertPosLoc;
GLint phong_vertNorLoc;
// Shader uniforms
// Vertex shader uniforms
GLint phong_modelLoc;
GLint phong_viewLoc;
GLint phong_projectionLoc;
// Fragment shader uniforms
GLint phong_camPosLoc;
GLint phong_materialAmbientLoc;
GLint phong_materialDiffuseLoc;
GLint phong_materialSpecularLoc;
GLint phong_materialShininessLoc;
GLint phong_lightPositionLoc;
GLint phong_lightAmbientLoc;
GLint phong_lightDiffuseLoc;
GLint phong_lightSpecularLoc;

// TODO: Change all shaders into similar format
struct OmpShader ompShader;
struct OcShader ocShader;
struct SbShader sbShader;

// Height of window ???
int g_width = 1280;
int g_height = 960;

// Framebuffer for postprocessing
unsigned int fbo;
unsigned int fbo_color_texture;
unsigned int fbo_depth_stencil_texture;

// Cubemaps
unsigned int cubemapTexture;
unsigned int skyTexture;

// For debugging
void printMatrix(glm::mat4 mat) {
  int i, j;
  for(i = 0; i < 4; i++) {
    for(j = 0; j < 4; j++) {
      printf("mat[%d][%d]: %f\n", i, j, mat[i][j]);
    }
  }
  printf("\n");
}

static void error_callback(int error, const char *description) {
  std::cerr << description << std::endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, 
  int mods) {
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else if(key == GLFW_KEY_W) {
    if(action == GLFW_PRESS) {
      keys[0] = 1;
    } else if(action == GLFW_RELEASE) {
      keys[0] = 0;
    }
  } else if(key == GLFW_KEY_A) {
    if(action == GLFW_PRESS) {
      keys[1] = 1;
    } else if(action == GLFW_RELEASE) {
      keys[1] = 0;
    }
  } else if(key == GLFW_KEY_S) {
    if(action == GLFW_PRESS) {
      keys[2] = 1;
    } else if(action == GLFW_RELEASE) {
      keys[2] = 0;
    }
  } else if(key == GLFW_KEY_D) {
    if(action == GLFW_PRESS) {
      keys[3] = 1;
    } else if(action == GLFW_RELEASE) {
      keys[3] = 0;
    }
  } else if(key == GLFW_KEY_Q) {
    if(action == GLFW_PRESS) {
      keys[4] = 1;
    } else if(action == GLFW_RELEASE) {
      keys[4] = 0;
    }
  } else if(key == GLFW_KEY_E) {
    if(action == GLFW_PRESS) {
      keys[5] = 1;
    } else if(action == GLFW_RELEASE) {
      keys[5] = 0;
    }
  }
}

static void resize_callback(GLFWwindow *window, int width, int height) {
  g_width = width;
  g_height = height;
  glViewport(0, 0, width, height);
}

char *textfileRead(const char *fn) {
  FILE *fp;
  char *content = NULL;
  int count = 0;
  if(fn != NULL) {
    fp = fopen(fn, "rt");
    if(fp != NULL) {
      fseek(fp, 0, SEEK_END);
      count = (int) ftell(fp);
      rewind(fp);
      if(count > 0) {
        content = (char *) malloc(sizeof(char) * (count + 1));
        count = (int) fread(content, sizeof(char), count, fp);
        content[count] = '\0';
      }
      fclose(fp);
    } else {
      printf("error loading %s\n", fn);
    }
  }
  return content;
}

// vsfn = vertex shader file name, fsfn = fragment shader file name
// Returns the shader program ID
GLuint initShader(const char *vsfn, const char *fsfn) {
  GLuint pid;
  GLint rc;
  GLuint vsHandle, fsHandle;
  char *vsSource, *fsSource;

  // Create shader handles
  vsHandle = glCreateShader(GL_VERTEX_SHADER);
  fsHandle = glCreateShader(GL_FRAGMENT_SHADER);
  
  // Read shader source
  vsSource = textfileRead(vsfn);
  fsSource = textfileRead(fsfn);

  glShaderSource(vsHandle, 1, &vsSource, NULL);
  glShaderSource(fsHandle, 1, &fsSource, NULL);

  free(vsSource);
  free(fsSource);

  // Compile vertex shader
  glCompileShader(vsHandle);
  glGetShaderiv(vsHandle, GL_COMPILE_STATUS, &rc);

  if(!rc) {
    std::cout << "Error compiling vertex shader" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Compile fragment shader
  glCompileShader(fsHandle);
  glGetShaderiv(fsHandle, GL_COMPILE_STATUS, &rc);

  if(!rc) {
    std::cout << "Error compiling fragment shader" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Create program and link
  pid = glCreateProgram();
  glAttachShader(pid, vsHandle);
  glAttachShader(pid, fsHandle);
  glLinkProgram(pid);
  glGetProgramiv(pid, GL_LINK_STATUS, &rc);

  if(!rc) {
    std::cout << "Error linking shaders" << std::endl;
    exit(EXIT_FAILURE);
  }

  return pid;
}

static void resizeMesh(std::vector<float>& posBuf) {
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

  scaleX = 2.0 /maxExtent;
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

static void getMesh(const std::string &meshName) {
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

// Store rectangle data in the buffers
static void getRectangleMesh() {
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

// Store skybox data in the buffers
static void getSkyboxMesh() {
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
  copy(&posArr[0], &posArr[18 * 6], back_inserter(posBuf));
}

// Store data about mesh
// NOTE: Mesh must have element buffer if passed into this function
static void sendMesh(unsigned *posBufID, unsigned *eleBufID,
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

// Send skybox data to GPU
static void sendSkyboxMesh() {
  // Send vertex position array to GPU
  glGenBuffers(1, &skybox_posBufID);
  glBindBuffer(GL_ARRAY_BUFFER, skybox_posBufID);
  glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0],
    GL_STATIC_DRAW);

  // Unbind arrays
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // This data is not on the GPU
  skybox_posBufSize = posBuf.size();
}

// Parameter is a vector of strings that are the file names
// Taken from https://learnopengl.com/Advanced-OpenGL/Cubemaps
unsigned int loadCubemap(std::vector<std::string> faces) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data = stbi_load(faces[i].c_str(), &width, &height,
      &nrChannels, 0);
    if(data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
        0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap texture failed to load at path: " <<
        faces[i] << std::endl;
      stbi_image_free(data);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // Unbind texture
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  return textureID;
}

static void init() {
  // Set background color
  glClearColor(.125f, .375f, .5f, 0.f);
  // Enable z-buffer test
  glEnable(GL_DEPTH_TEST);
  // Stencil test
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  // Something about blending ???
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  // For skybox
  glDepthFunc(GL_LEQUAL);

  // Create framebuffer object with only colors
  glGenFramebuffers(1, &fbo);
  // Bind
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // Color texture for fbo
  glGenTextures(1, &fbo_color_texture);
  glBindTexture(GL_TEXTURE_2D, fbo_color_texture);
  // Multiply by level for super sample anti-aliasing
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_width * ssaaLevel,
    g_height * ssaaLevel, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Attach texture to fbo
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
    fbo_color_texture, 0);
  // Unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  // TODO : attach depth and stencil separately
  // Depth and stencil texture for fbo
  glGenTextures(1, &fbo_depth_stencil_texture);
  glBindTexture(GL_TEXTURE_2D, fbo_depth_stencil_texture);
  // Multiply by level for super sample anti-aliasing
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, g_width * ssaaLevel, 
    g_height * ssaaLevel, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
  );
  // Attach texture to fbo
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
    GL_TEXTURE_2D, fbo_depth_stencil_texture, 0);  
  // Unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  // Check for completion
  if(!(glCheckFramebufferStatus(GL_FRAMEBUFFER) ==
    GL_FRAMEBUFFER_COMPLETE)) {
    printf("Incomplete second framebuffer\n");
    exit(1);
  }

  // Unbind
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Get mesh
  getMesh("../resources/sphere.obj");
  resizeMesh(posBuf);

  // Send mesh to GPU and store buffer IDs
  sendMesh(&sphere_posBufID, &sphere_eleBufID, &sphere_texCoordBufID,
    &sphere_eleBufSize, &sphere_norBufID);

  // Do again for bunny
  getMesh("../resources/bunny.obj");
  resizeMesh(posBuf);
  sendMesh(&bunny_posBufID, &bunny_eleBufID, NULL,
    &bunny_eleBufSize, &bunny_norBufID);

  // Do again for rectangle
  getRectangleMesh();
  // No need to resize, explicitly specified coordinates
  sendMesh(&rect_posBufID, &rect_eleBufID, &rect_texCoordBufID,
    &rect_eleBufSize, NULL);

  // Do again for cube
  getSkyboxMesh();
  sendSkyboxMesh();

  // Read textures into CPU memory
  struct Image image;

  // For some reason stb loads images upside-down to how we want
  stbi_set_flip_vertically_on_load(true);

  // Load image
  image.data = stbi_load("../resources/world.bmp", &(image.sizeX),
    &(image.sizeY), &(image.numChannels), 0);

  // Allocate space on GPU then load texture into the GPU

  // Set the first texture unit as active
  glActiveTexture(GL_TEXTURE0);
  // Generate texture buffer object
  glGenTextures(1, &sphere_texBufID);
  // Bind current texture unit to texture buffer object as a GL_TEXTURE_2D
  glBindTexture(GL_TEXTURE_2D, sphere_texBufID);
  // Load texture data into texBufID
  // Base level is 0, number of channels is 3, and border is 0
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.sizeX, image.sizeY,
    0, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte *) image.data);

  // Generate image pyramid
  glGenerateMipmap(GL_TEXTURE_2D);
  // Set texture wrap modes for S and T directions
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // Set filtering mode for magnification and minification
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR_MIPMAP_LINEAR);

  // Unbind from texture buffer object from current texture unit
  glBindTexture(GL_TEXTURE_2D, 0);

  // Clear image to prepare loading another image to GPU
  free(image.data);

  // Load image
  image.data = stbi_load("../resources/grass.png", &(image.sizeX),
    &(image.sizeY), &(image.numChannels), 0);
  
  // Set the first texture unit as active
  glActiveTexture(GL_TEXTURE0);
  // Generate texture buffer object
  glGenTextures(1, &grass_texBufID);
  // Bind current texture unit to texture buffer object as a GL_TEXTURE_2D
  glBindTexture(GL_TEXTURE_2D, grass_texBufID);
  // Load texture data into texBufID
  // Base level is 0, number of channels is 3, and border is 0
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.sizeX, image.sizeY,
    0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte *) image.data);

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
  free(image.data);

  // Cubemap images are upside down
  stbi_set_flip_vertically_on_load(false);
  // Load cubemap
  std::vector<std::string> faces;
  faces = {
    "../resources/skybox/right.jpg",
    "../resources/skybox/left.jpg",
    "../resources/skybox/top.jpg",
    "../resources/skybox/bottom.jpg",
    "../resources/skybox/front.jpg",
    "../resources/skybox/back.jpg"
  };
  cubemapTexture = loadCubemap(faces);
  faces = {
    "../resources/skybox/top.jpg",
    "../resources/skybox/top.jpg",
    "../resources/skybox/top.jpg",
    "../resources/skybox/top.jpg",
    "../resources/skybox/top.jpg",
    "../resources/skybox/top.jpg"
  };
  skyTexture = loadCubemap(faces);

  // Initialize shader programs

  // Texture only shader program

  to_pid = initShader("../resources/vertTextureOnly.glsl",
    "../resources/fragTextureOnly.glsl");

  // Attribs
  to_vertPosLoc = glGetAttribLocation(to_pid, "vertPos");
  to_texCoordLoc = glGetAttribLocation(to_pid, "texCoord");

  // Per-object matrices to pass to shaders
  // TODO: Replace perspective and placement with modelview and projection
  to_modelviewLoc = glGetUniformLocation(to_pid, "modelview");
  to_projectionLoc = glGetUniformLocation(to_pid, "projection");

  // Get the location of the sampler2D in fragment shader (???)
  to_texLoc = glGetUniformLocation(to_pid, "texCol");

  // VAO for globe
  // Create vertex array object
  glGenVertexArrays(1, &to_vaoID);
  glBindVertexArray(to_vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(to_vertPosLoc);
  glBindBuffer(GL_ARRAY_BUFFER, sphere_posBufID);
  glVertexAttribPointer(to_vertPosLoc, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind texture coordinate buffer
  glEnableVertexAttribArray(to_texCoordLoc);
  glBindBuffer(GL_ARRAY_BUFFER, sphere_texCoordBufID);
  glVertexAttribPointer(to_texCoordLoc, 2, GL_FLOAT, GL_FALSE, 
    sizeof(GL_FLOAT) * 2, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(to_vertPosLoc);
  glDisableVertexAttribArray(to_texCoordLoc);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // VAO for grass
  // Create vertex array object
  glGenVertexArrays(1, &grass_vaoID);
  glBindVertexArray(grass_vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(to_vertPosLoc);
  glBindBuffer(GL_ARRAY_BUFFER, rect_posBufID);
  glVertexAttribPointer(to_vertPosLoc, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind texture coordinate buffer
  glEnableVertexAttribArray(to_texCoordLoc);
  glBindBuffer(GL_ARRAY_BUFFER, rect_texCoordBufID);
  glVertexAttribPointer(to_texCoordLoc, 2, GL_FLOAT, GL_FALSE, 
    sizeof(GL_FLOAT) * 2, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(to_vertPosLoc);
  glDisableVertexAttribArray(to_texCoordLoc);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Depth Only shader program

  do_pid = initShader("../resources/vertDepthOnly.glsl",
    "../resources/fragDepthOnly.glsl");

  // Attribs
  do_vertPosLoc = glGetAttribLocation(do_pid, "vertPos");

  // Per-object matrices to pass to shaders
  // TODO: Replace perspective and placement with modelview and projection
  do_modelviewLoc = glGetUniformLocation(do_pid, "modelview");
  do_projectionLoc = glGetUniformLocation(do_pid, "projection");

  // Bunny vertex array object

  // Create vertex array object
  glGenVertexArrays(1, &do_vaoID);
  glBindVertexArray(do_vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(do_vertPosLoc);
  glBindBuffer(GL_ARRAY_BUFFER, bunny_posBufID);
  glVertexAttribPointer(do_vertPosLoc, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunny_eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(do_vertPosLoc);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Sphere vertex array object

  // Create vertex array object
  glGenVertexArrays(1, &do_sphere_vaoID);
  glBindVertexArray(do_sphere_vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(do_vertPosLoc);
  glBindBuffer(GL_ARRAY_BUFFER, sphere_posBufID);
  glVertexAttribPointer(do_vertPosLoc, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(do_vertPosLoc);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Rectangle shader program

  r_pid = initShader("../resources/vertRectangle.glsl",
    "../resources/fragRectangle.glsl");

  // Attribs
  r_vertPosLoc = glGetAttribLocation(r_pid, "vertPos");
  r_texCoordLoc = glGetAttribLocation(r_pid, "texCoord");

  // Get the location of the sampler2D in fragment shader (???)
  r_texLoc = glGetUniformLocation(r_pid, "texCol");

  // Create vertex array object
  glGenVertexArrays(1, &rect_vaoID);
  glBindVertexArray(rect_vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(r_vertPosLoc);
  glBindBuffer(GL_ARRAY_BUFFER, rect_posBufID);
  glVertexAttribPointer(r_vertPosLoc, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind texture coordinate buffer
  glEnableVertexAttribArray(r_texCoordLoc);
  glBindBuffer(GL_ARRAY_BUFFER, rect_texCoordBufID);
  glVertexAttribPointer(r_texCoordLoc, 2, GL_FLOAT, GL_FALSE, 
    sizeof(GL_FLOAT) * 2, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(r_vertPosLoc);
  glDisableVertexAttribArray(r_texCoordLoc);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Cubemap shader program

  sbShader.pid = initShader("../resources/vertCubemap.glsl",
    "../resources/fragCubemap.glsl");

  // Attribs
  sbShader.vertPos = glGetAttribLocation(sbShader.pid, "vertPos");

  // Per-object matrices to pass to shaders
  // TODO: Replace perspective and placement with modelview and projection
  sbShader.modelview = glGetUniformLocation(sbShader.pid, "modelview");
  sbShader.projection = glGetUniformLocation(sbShader.pid, "projection");

  // Get the location of the samplerCube in fragment shader (???)
  // TODO: change the names to match
  sbShader.texLoc = glGetUniformLocation(sbShader.pid, "skybox");

  // Skybox vertex array object

  // Create vertex array object
  glGenVertexArrays(1, &skybox_vaoID);
  glBindVertexArray(skybox_vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(sbShader.vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, skybox_posBufID);
  glVertexAttribPointer(sbShader.vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(sbShader.vertPos);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // One color shader program

  ocShader.pid = initShader("../resources/vertOneColor.glsl",
    "../resources/fragOneColor.glsl");

  // Attribs
  ocShader.vertPos = glGetAttribLocation(ocShader.pid, "vertPos");

  // Per-object matrices to pass to shaders
  ocShader.modelview = glGetUniformLocation(ocShader.pid, "modelview");
  ocShader.projection = glGetUniformLocation(ocShader.pid, "projection");
  ocShader.in_color = glGetUniformLocation(ocShader.pid, "in_color");

  // Pink bunny vertex array object

  // Create vertex array object
  glGenVertexArrays(1, &oc_bunny_vaoID);
  glBindVertexArray(oc_bunny_vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(ocShader.vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, bunny_posBufID);
  glVertexAttribPointer(ocShader.vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunny_eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(ocShader.vertPos);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Light source vertex array object

  // Create vertex array object
  glGenVertexArrays(1, &ls_vaoID);
  glBindVertexArray(ls_vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(ocShader.vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, sphere_posBufID);
  glVertexAttribPointer(ocShader.vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(ocShader.vertPos);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // One material phong shader program

  ompShader.pid = initShader("../resources/vertOneMaterialPhong.glsl",
    "../resources/fragOneMaterialPhong.glsl");

  // Attribs
  ompShader.vertPos = glGetAttribLocation(ompShader.pid, "vertPos");
  ompShader.vertNor = glGetAttribLocation(ompShader.pid, "vertNor");

  // Per-object matrices to pass to shaders
  // Vertex shader uniforms
  ompShader.model = glGetUniformLocation(ompShader.pid, "model");
  ompShader.view = glGetUniformLocation(ompShader.pid, "view");
  ompShader.projection = glGetUniformLocation(ompShader.pid, "projection");
  // Fragment shader uniforms
  ompShader.camPos = glGetUniformLocation(ompShader.pid, "cam_pos");
  ompShader.materialAmbient = glGetUniformLocation(ompShader.pid,
    "material.ambient");
  ompShader.materialDiffuse = glGetUniformLocation(ompShader.pid,
    "material.diffuse");
  ompShader.materialSpecular = glGetUniformLocation(ompShader.pid,
    "material.specular");
  ompShader.materialShininess = glGetUniformLocation(ompShader.pid,
    "material.shininess");
  ompShader.lightPosition = glGetUniformLocation(ompShader.pid,
    "light.position");
  ompShader.lightAmbient = glGetUniformLocation(ompShader.pid,
    "light.ambient");
  ompShader.lightDiffuse = glGetUniformLocation(ompShader.pid,
    "light.diffuse");
  ompShader.lightSpecular = glGetUniformLocation(ompShader.pid,
    "light.specular");

  // One Material Phong bunny VAO

  // Create vertex array object
  glGenVertexArrays(1, &omp_bunny_vaoID);
  glBindVertexArray(omp_bunny_vaoID);

  // Bind position buffer
  glEnableVertexAttribArray(ompShader.vertPos);
  glBindBuffer(GL_ARRAY_BUFFER, bunny_posBufID);
  glVertexAttribPointer(ompShader.vertPos, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);
  
  // Bind normal buffer
  glEnableVertexAttribArray(ompShader.vertNor);
  glBindBuffer(GL_ARRAY_BUFFER, bunny_norBufID);
  glVertexAttribPointer(ompShader.vertNor, 3, GL_FLOAT, GL_FALSE,
    sizeof(GL_FLOAT) * 3, (const void *) 0);

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunny_eleBufID);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Disable
  glDisableVertexAttribArray(ompShader.vertPos);
  glDisableVertexAttribArray(ompShader.vertNor);

  // Unbind GPU buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // TODO: General phong shader
  // Phong shader program

  phong_pid = initShader("../resources/vertPhong.glsl",
    "../resources/fragPhong.glsl");

  // Attribs
  phong_vertPosLoc = glGetAttribLocation(phong_pid, "vertPos");
  phong_vertNorLoc = glGetAttribLocation(phong_pid, "vertNor");

  // Per-object matrices to pass to shaders
  // Vertex shader uniforms
  phong_modelLoc = glGetUniformLocation(phong_pid, "model");
  phong_viewLoc = glGetUniformLocation(phong_pid, "view");
  phong_projectionLoc = glGetUniformLocation(phong_pid, "projection");
  // Fragment shader uniforms
  phong_camPosLoc = glGetUniformLocation(phong_pid, "cam_pos");
  phong_materialAmbientLoc = glGetUniformLocation(phong_pid,
    "material.ambient");
  phong_materialDiffuseLoc = glGetUniformLocation(phong_pid,
    "material.diffuse");
  phong_materialSpecularLoc = glGetUniformLocation(phong_pid,
    "material.specular");
  phong_materialShininessLoc = glGetUniformLocation(phong_pid,
    "material.shininess");
  phong_lightPositionLoc = glGetUniformLocation(phong_pid,
    "light.position");
  phong_lightAmbientLoc = glGetUniformLocation(phong_pid,
    "light.ambient");
  phong_lightDiffuseLoc = glGetUniformLocation(phong_pid,
    "light.diffuse");
  phong_lightSpecularLoc = glGetUniformLocation(phong_pid,
    "light.specular");

  // TODO: Cube vertex array object
}

static void render() {
  int width, height;
  float aspect;
  glm::dvec2 cursorPos;
  glm::dvec2 screenPos;

  // Create matrices
  glm::mat4 matPlacement;
  glm::mat4 matPerspective;
  glm::mat4 matCamera;

  glm::mat4 matModel;
  glm::mat4 matView;
  glm::mat4 matModelview;
  glm::mat4 matProjection;

  // Get current frame buffer size ???
  glfwGetFramebufferSize(window, &width, &height);

  // Change framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  // Tells to what dimensions it renders to
  glViewport(0, 0, width * ssaaLevel, height * ssaaLevel);
  
  // Buffer stuff
  glEnable(GL_DEPTH_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  // Clear framebuffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Detect and set the current rotation of the camera
  glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);
  screenPos.x = cursorPos.x / (double) width;
  screenPos.y = cursorPos.y / (double) height;

  // Rotation along y axis
  if(screenPos.x > .9f && screenPos.x < 1.f &&
    screenPos.y > 0.f && screenPos.y < 1.f) {
    camRotation.y = camRotation.y - .01f;
  } else if(screenPos.x < .1f && screenPos.x > 0.f &&
    screenPos.y > 0.f && screenPos.y < 1.f) {
    camRotation.y = camRotation.y + .01f;
  }

  if(camRotation.y <= 0.00001f) {
    camRotation.y = 2.f * PI;
  } else if(camRotation.y >= 2.f * PI) {
    camRotation.y = 0.00001f;
  }

  // Rotation along x axis
  // NOTE POSITIVE DIRECTION
  if(screenPos.y > .9f && screenPos.y < 1.f &&
    camRotation.x > -PI / 2.f &&
    screenPos.x > 0.f && screenPos.x < 1.f) {
    camRotation.x = camRotation.x - .01f;
  } else if(screenPos.y < .1f && screenPos.y > 0.f &&
    camRotation.x < PI / 2.f &&
    screenPos.x > 0.f && screenPos.x < 1.f) {
    camRotation.x = camRotation.x + .01f;
  }

  // Update the forward direction
  forward = glm::vec3(glm::rotate(glm::mat4(1.f), camRotation.y,
    glm::vec3(0.f, 1.f, 0.f)) *
    glm::rotate(glm::mat4(1.f), camRotation.x,
    glm::vec3(1.f, 0.f, 0.f)) *
    glm::vec4(0.f, 0.f, -1.f, 1.f));
  forward = glm::normalize(glm::vec3(forward.x, 0.f, forward.z));
  // Update the side direction
  sideways = glm::vec3(glm::rotate(glm::mat4(1.f), camRotation.y,
    glm::vec3(0.f, 1.f, 0.f)) *
    glm::rotate(glm::mat4(1.f), camRotation.x,
    glm::vec3(1.f, 0.f, 0.f)) *
    glm::vec4(1.f, 0.f, 0.f, 1.f));
  sideways = glm::normalize(glm::vec3(sideways.x, 0.f, sideways.z));
  // Update position
  if(keys[0]) {
    camLocation = camLocation + forward * 0.05f;
  }
  if(keys[1]) {
    camLocation = camLocation - sideways * 0.05f;
  }
  if(keys[2]) {
    camLocation = camLocation - forward * 0.05f;
  }
  if(keys[3]) {
    camLocation = camLocation + sideways * 0.05f;
  }
  if(keys[4]) {
    camLocation = camLocation + glm::vec3(0.f, 0.05f, 0.f);
  }
  if(keys[5]) {
    camLocation = camLocation - glm::vec3(0.f, 0.05f, 0.f);
  }

  // Perspective matrix
  aspect = width / (float) height;
  matPerspective = glm::perspective(70.f, aspect, .1f, 10.f);

  matProjection = matPerspective;

  // Transformations regarding the camera
  matCamera = glm::mat4(1.f);
  matCamera = glm::translate(glm::mat4(1.f), -camLocation) *
    matCamera;

  matCamera = glm::rotate(glm::mat4(1.f), -camRotation.x,
    sideways) * matCamera;
  matCamera = glm::rotate(glm::mat4(1.f), -camRotation.y,
    glm::vec3(0.f, 1.f, 0.f)) * matCamera;

  matView = matCamera;

  // Draw the globe

  // Stencil for outline
  // GL_ALWAYS = Never discard the fragment, GL_REPLACE means 1 is
  // put into stencil buffer
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  // Value you put into stencil buffer is ANDed with 0xFF
  glStencilMask(0xFF);
  
  // Model matrix
  matModel = glm::mat4(1.f);

  // Move object relative to the world
  matModel = glm::scale(glm::mat4(1.f),
    glm::vec3(1.f, 1.f, 1.f)) * 
    matModel;
  
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;
  
  // Object position is (0, 0, -2)
  matModel = glm::translate(glm::mat4(1.f),
    glm::vec3(0.f, 0.f, -2.f)) * matModel;

  // Move object relative to the eye
  matModelview = matView * matModel;

  // Bind shader program
  glUseProgram(to_pid);

  // Fill in matrices
  glUniformMatrix4fv(to_modelviewLoc, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(to_projectionLoc, 1, GL_FALSE,
    glm::value_ptr(matProjection));

  // Bind vertex array object
  glBindVertexArray(to_vaoID);

  // Bind texture to texture unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sphere_texBufID);
  // 0 because texture unit GL_TEXTURE0
  glUniform1i(to_texLoc, 0);

  // Draw one object
  glDrawElements(GL_TRIANGLES, sphere_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Draw the sphere outline

  // Stencil for outline
  // GL_NOTEQUAL = Don't discard the fragment, if stencil
  // is not equal to 1
  // Determines if fragment should pass stencil test
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  // Value you put into stencil buffer is ANDed with 0x00
  glStencilMask(0x00);

  // Placement matrix
  matModel = glm::mat4(1.f);

  // Put object into world
  matModel = glm::scale(glm::mat4(1.f),
    glm::vec3(1.25f, 1.25f, 1.25f)) * 
    matModel;
  
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;
  
  // Object position is (0, 0, -2)
  matModel = glm::translate(glm::mat4(1.f),
    glm::vec3(0.f, 0.f, -2.f)) * matModel;
  
  // Modify object relative to the eye
  matModelview = matView * matModel;

  // Bind shader program
  glUseProgram(do_pid);

  // Fill in matrices
  glUniformMatrix4fv(do_modelviewLoc, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(do_projectionLoc, 1, GL_FALSE,
    glm::value_ptr(matProjection));

  // Bind vertex array object
  glBindVertexArray(do_sphere_vaoID);

  // Draw one object
  glDrawElements(GL_TRIANGLES, sphere_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);
  // REMINDER: Changed element buffer size

  // Unbind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Draw the grass
  
  // Do nothing to the stencil buffer ever
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  // Placement matrix
  matModel = glm::mat4(1.f);

  // Put object into world
  matModel = glm::scale(glm::mat4(1.f),
    glm::vec3(1.f, 1.f, 1.f)) * 
    matModel;
  
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  // Object position is (4, 0, -2)
  matModel = glm::translate(glm::mat4(1.f),
    glm::vec3(4.f, 0.f, -2.f)) * matModel;

  // Modify object relative to the eye
  matModelview = matView * matModel;

  // Bind shader program
  glUseProgram(to_pid);

  // Fill in matrices
  glUniformMatrix4fv(to_modelviewLoc, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(to_projectionLoc, 1, GL_FALSE,
    glm::value_ptr(matProjection));

  // Bind vertex array object
  glBindVertexArray(grass_vaoID);

  // Bind texture to texture unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, grass_texBufID);
  // 0 because texture unit GL_TEXTURE0
  glUniform1i(r_texLoc, 0);

  // Draw one object
  glDrawElements(GL_TRIANGLES, rect_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Draw the one color bunny

  // Do nothing to the stencil buffer ever
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  // Placement matrix
  matModel = glm::mat4(1.f);
  
  // Put object into world
  matModel = glm::scale(glm::mat4(1.f),
    glm::vec3(1.f, 1.f, 1.f)) * 
    matModel;
  
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  // Object position is (-4, 0, -2)
  matModel = glm::translate(glm::mat4(1.f),
    glm::vec3(-4.f, 0.f, -2.f)) * matModel;

  // Modify object relative to the eye
  matModelview = matView * matModel;

  // Bind shader program
  glUseProgram(ocShader.pid);

  // Fill in matrices
  glUniformMatrix4fv(ocShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(ocShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  // Bunny is red (1.0, 0.0, 0.0)
  glUniform3fv(ocShader.in_color, 1,
    glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));

  // Bind vertex array object
  glBindVertexArray(oc_bunny_vaoID);

  // Draw one object
  glDrawElements(GL_TRIANGLES, bunny_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Draw the light source

  // Do nothing to the stencil buffer ever
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  // Placement matrix
  matModel = glm::mat4(1.f);

  // Put object into world
  matModel = glm::scale(glm::mat4(1.f),
    glm::vec3(.5f, .5f, .5f)) * 
    matModel;
  
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  // Object position is (-8, 0, -2)
  matModel = glm::translate(glm::mat4(1.f),
    tutorialLight.position) * matModel;

  // Move object relative to the eye
  matModelview = matView * matModel;

  // Bind shader program
  glUseProgram(ocShader.pid);

  // Fill in matrices
  glUniformMatrix4fv(ocShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(ocShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  glUniform3fv(ocShader.in_color, 1,
    glm::value_ptr(tutorialLight.specular));

  // Bind vertex array object
  glBindVertexArray(ls_vaoID);

  // Draw one object
  glDrawElements(GL_TRIANGLES, sphere_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Draw the phong bunny

  // Do nothing to the stencil buffer ever
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  // Placement matrix
  matModel = glm::mat4(1.f);

  // Put object into world
  matModel = glm::scale(glm::mat4(1.f),
    glm::vec3(1.f, 1.f, 1.f)) * 
    matModel;

  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  // Object position is (-12, 0, -2)
  matModel = glm::translate(glm::mat4(1.f),
    glm::vec3(-12.f, 0.f, -2.f)) * matModel;

  // Bind shader program
  // TODO: Light going behind object
  glUseProgram(ompShader.pid);

  // Bind vertex array object
  glBindVertexArray(omp_bunny_vaoID);

  // Fill in matrices
  // Fill in vertex shader uniforms
  // TODO: Change to modelview matrix
  // TODO: Lighting calculation in view space
  glUniformMatrix4fv(ompShader.model, 1, GL_FALSE,
    glm::value_ptr(matModel));
  glUniformMatrix4fv(ompShader.view, 1, GL_FALSE,
    glm::value_ptr(matView));
  glUniformMatrix4fv(ompShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  // Fill in fragment shader uniforms
  glUniform3fv(ompShader.camPos, 1,
    glm::value_ptr(camLocation));
  glUniform3fv(ompShader.materialAmbient, 1,
    glm::value_ptr(copper.ambient));
  glUniform3fv(ompShader.materialDiffuse, 1,
    glm::value_ptr(copper.diffuse));
  glUniform3fv(ompShader.materialSpecular, 1,
    glm::value_ptr(copper.specular));
  glUniform1f(ompShader.materialShininess,
    copper.shininess);
  glUniform3fv(ompShader.lightPosition, 1,
    glm::value_ptr(tutorialLight.position));
  glUniform3fv(ompShader.lightAmbient, 1,
    glm::value_ptr(tutorialLight.ambient));
  glUniform3fv(ompShader.lightDiffuse, 1,
    glm::value_ptr(tutorialLight.diffuse));
  glUniform3fv(ompShader.lightSpecular, 1,
    glm::value_ptr(tutorialLight.specular));

  // Draw one object
  glDrawElements(GL_TRIANGLES, bunny_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Draw the cubemap

  // Do nothing to the stencil buffer ever
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  // Placement matrix
  matModel = glm::mat4(1.f);

  // Put object into world
  matModel = glm::scale(glm::mat4(1.f),
    glm::vec3(1.f, 1.f, 1.f)) * 
    matModel;
  
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  // Object position is the location of the camera
  matModel = glm::translate(glm::mat4(1.f),
    camLocation) * matModel;

  // Modify object relative to the eye
  matModelview = matView * matModel;

  // Bind shader program
  glUseProgram(sbShader.pid);

  // Fill in matrices
  glUniformMatrix4fv(sbShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(sbShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));

  // Bind vertex array object
  glBindVertexArray(skybox_vaoID);

  // Texture unit example below
  // Bind the texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
  // TESTING
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
  // 1 because correct skybox is in texture unit GL_TEXTURE1
  glUniform1i(sbShader.texLoc, 1);

  // Draw the cube
  // Divide by 3 because per vertex
  glDrawArrays(GL_TRIANGLES, 0, skybox_posBufSize / 3);

  // Unbind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Paste from side framebuffer to default framebuffer

  // Bind normal framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  // Draw the rectangle

  // Will always pass stencil test
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  // Never write to the stencil buffer
  glStencilMask(0x00);

  // Bind shader program
  glUseProgram(r_pid);

  // Bind vertex array object
  glBindVertexArray(rect_vaoID);

  // Bind texture to texture unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fbo_color_texture);
  // 0 because texture unit GL_TEXTURE0
  glUniform1i(r_texLoc, 0);

  // Draw one object
  glDrawElements(GL_TRIANGLES, rect_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Re-Enable
  glStencilMask(0xFF);
  glEnable(GL_DEPTH_TEST);
}

int main(int argc, char **argv) {
  // What function to call when there is an error
  glfwSetErrorCallback(error_callback);

  // Initialize GLFW
  if(glfwInit() == false) {
    return -1;
  }

  // ???
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  // Create a windowed mode window and (?) its OpenGL context. (?)
  window = glfwCreateWindow(g_width, g_height, "Some title", NULL, NULL);
  if(window == false) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = true;
  if(glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
  }

  // Bootstrap ???
  glGetError();
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) <<
    std::endl;

  // Set vsync ???
  glfwSwapInterval(1);
  // Set callback(s) for window
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, resize_callback);

  // TESTING
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  // Depth and stencil masks
  glDepthMask(GL_TRUE);
  glStencilMask(0xFF);

  // Initialize scene
  init();

  // Loop until the user closes the window
  while(!glfwWindowShouldClose(window)) {
    // Render scene
    render();
    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
  }

  // Quit program
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

