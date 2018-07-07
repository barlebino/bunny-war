#include <iostream>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#define PI 3.14159

#include "tiny_obj_loader.h"

#include <unistd.h>

// Image code, for textures
struct Image {
  int sizeX, sizeY;
  char *data;
};

struct RGB {
  GLubyte r, g, b;
};

GLFWwindow *window; // Main application window

// Location of camera
glm::vec3 camLocation = glm::vec3(0.f, 0.f, 2.f);
// Rotation of camera
glm::vec2 camRotation = glm::vec2(0.f, 0.f);
// Forward direction
glm::vec3 forward = glm::vec3(0.f, 0.f, -1.f);
// Sideways direction
glm::vec3 sideways = glm::vec3(1.f, 0.f, 0.f);

// Input
char keys[6] = {0, 0, 0, 0, 0, 0};

// CPU buffers
std::vector<float> posBuf;
std::vector<float> norBuf;
std::vector<float> texCoordBuf;
std::vector<unsigned int> eleBuf;

// Buffer IDs
unsigned to_vaoID;
unsigned do_vaoID;
unsigned do_sphere_vaoID;

// Sphere data
unsigned sphere_posBufID;
unsigned sphere_eleBufID;
unsigned sphere_texCoordBufID;
unsigned sphere_texBufID;
int sphere_eleBufSize;

// Bunny data
unsigned bunny_posBufID;
unsigned bunny_eleBufID;
int bunny_eleBufSize;

// Shader programs
GLuint to_pid; // Texture only

GLuint do_pid; // Depth only

// Shader attribs
GLint to_vertPosLoc;
GLint to_texCoordLoc;

GLint do_vertPosLoc;

// Shader uniforms
GLint to_perspectiveLoc;
GLint to_placementLoc;

GLint do_perspectiveLoc;
GLint do_placementLoc;

// sampler2D location
GLint to_texLoc;

// Height of window ???
int g_width = 640;
int g_height = 480;

// Framebuffer for postprocessing
unsigned int fbo;
unsigned int fbo_color_texture;
unsigned int fbo_depth_stencil_texture;

// Helper functions for image load
static unsigned int getint(FILE *fp) {
  int c, c1, c2, c3;
	
  // Get 4 bytes
  c = getc(fp);
  c1 = getc(fp);
  c2 = getc(fp);
  c3 = getc(fp);

	return ((unsigned int) c) + (((unsigned int) c1) << 8) +
	(((unsigned int) c2) << 16) + (((unsigned int) c3) << 24);
}

static unsigned int getshort(FILE *fp) {
	int c, c1;
	
	// Get 2 bytes
	c = getc(fp);
	c1 = getc(fp);
	
	return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

int imageLoad(const char *filename, Image *image) {
	FILE *file;
	unsigned long size; // Size of the image in bytes
	unsigned long i; // Standard counter
	unsigned short int planes; // Number of planes in image (must be 1)
	unsigned short int bpp; // Number of bits per pixel (must be 24)
	char temp; // Used to convert bgr to rgb color
	
	// Make sure the file is there
	if((file = fopen(filename, "rb")) == NULL) {
		printf("File Not Found : %s\n", filename);
		return 0;
	}
	
	// Seek through the bmp header, up to the width height:
	fseek(file, 18, SEEK_CUR);
	
	// No 100% errorchecking anymore!!!
	
	// Read the width
  image->sizeX = getint(file);
	
	// Read the height
	image->sizeY = getint(file);
	
	// Calculate the size (assuming 24 bits or 3 bytes per pixel)
	size = image->sizeX * image->sizeY * 3;
	
	// Read the planes
	planes = getshort(file);
	if(planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
	}
	
	// Read the bpp
	bpp = getshort(file);
	if(bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}
	
	// Seek past the rest of the bitmap header
	fseek(file, 24, SEEK_CUR);
	
	// Read the data
	image->data = (char *) malloc(size);
	if(image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		return 0;
	}
	
	if((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}
	
	for(i = 0; i < size; i += 3) { // Reverse all of the colors (bgr -> rgb)
		temp = image->data[i];
		image->data[i] = image->data[i+2];
		image->data[i+2] = temp;
	}
	
	fclose(file); // Close the file and release the filedes
	
	// We're done
	return 1;
}

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
    texCoordBuf = shapes[0].mesh.texcoords;
		eleBuf = shapes[0].mesh.indices;
	}
}

// Store data about mesh
static void sendMesh(unsigned *posBufID, unsigned *eleBufID,
  unsigned *texCoordBufID, int *eleBufSize) {
  // Send vertex position array to GPU
  glGenBuffers(1, posBufID);
  glBindBuffer(GL_ARRAY_BUFFER, *posBufID);
  glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0],
    GL_STATIC_DRAW);

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

  // Create framebuffer object with only colors
  glGenFramebuffers(1, &fbo);

  // Color texture for fbo
  glGenTextures(1, &fbo_color_texture);
  glBindTexture(GL_TEXTURE_2D, fbo_color_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_width, g_height, 0, GL_RGB,
    GL_UNSIGNED_BYTE, NULL);
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
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, g_width, g_height, 0, 
    GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
  );
  // Attach texture to fbo
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
    GL_TEXTURE_2D, fbo_depth_stencil_texture, 0);  
  // Unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  // Get mesh
  getMesh("../resources/sphere.obj");
  resizeMesh(posBuf);

  // Send mesh to GPU and store buffer IDs
  sendMesh(&sphere_posBufID, &sphere_eleBufID, &sphere_texCoordBufID,
    &sphere_eleBufSize);

  // Do again for bunny
  getMesh("../resources/bunny.obj");
  resizeMesh(posBuf);
  sendMesh(&bunny_posBufID, &bunny_eleBufID, NULL,
    &bunny_eleBufSize);

  // Read texture into CPU memory
  struct Image image;
  imageLoad("../resources/world.bmp", &image);

  // Load the texture into the GPU

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
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR_MIPMAP_LINEAR);

  // Unbind from texture buffer object from current texture unit
  glBindTexture(GL_TEXTURE_2D, 0);

  // Create shader programs

  // Initialize shader program
  GLint rc;
  GLuint vsHandle, fsHandle;
  const char *vsSource, *fsSource;

  // Texture only shader program

  // Create shader handles
  vsHandle = glCreateShader(GL_VERTEX_SHADER);
  fsHandle = glCreateShader(GL_FRAGMENT_SHADER);

  // Read shader source code
  vsSource = textfileRead("../resources/vertTextureOnly.glsl");
  fsSource = textfileRead("../resources/fragTextureOnly.glsl");

  glShaderSource(vsHandle, 1, &vsSource, NULL);
  glShaderSource(fsHandle, 1, &fsSource, NULL);

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
  to_pid = glCreateProgram();
  glAttachShader(to_pid, vsHandle);
  glAttachShader(to_pid, fsHandle);
  glLinkProgram(to_pid);
  glGetProgramiv(to_pid, GL_LINK_STATUS, &rc);

  if(!rc) {
    std::cout << "Error linking shaders" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Attribs
  to_vertPosLoc = glGetAttribLocation(to_pid, "vertPos");
  to_texCoordLoc = glGetAttribLocation(to_pid, "texCoord");

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

  // Matrices to pass to vertex shaders
  to_perspectiveLoc = glGetUniformLocation(to_pid, "perspective");
  to_placementLoc = glGetUniformLocation(to_pid, "placement");

  // Get the location of the sampler2D in fragment shader (???)
  to_texLoc = glGetUniformLocation(to_pid, "texCol");

  // Depth Only shader program

  // Create shader handles
  vsHandle = glCreateShader(GL_VERTEX_SHADER);
  fsHandle = glCreateShader(GL_FRAGMENT_SHADER);

  // Read shader source code
  vsSource = textfileRead("../resources/vertDepthOnly.glsl");
  fsSource = textfileRead("../resources/fragDepthOnly.glsl");

  glShaderSource(vsHandle, 1, &vsSource, NULL);
  glShaderSource(fsHandle, 1, &fsSource, NULL);

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
  do_pid = glCreateProgram();
  glAttachShader(do_pid, vsHandle);
  glAttachShader(do_pid, fsHandle);
  glLinkProgram(do_pid);
  glGetProgramiv(do_pid, GL_LINK_STATUS, &rc);

  if(!rc) {
    std::cout << "Error linking shaders" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Attribs
  do_vertPosLoc = glGetAttribLocation(do_pid, "vertPos");

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

  // Matrices to pass to vertex shaders
  do_perspectiveLoc = glGetUniformLocation(do_pid, "perspective");
  do_placementLoc = glGetUniformLocation(do_pid, "placement");
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

  // Get current frame buffer size ???
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  // gluPerspective???

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

  // Transformations regarding the camera
  matCamera = glm::mat4(1.f);
  matCamera = glm::translate(glm::mat4(1.f), -camLocation) *
    matCamera;

  matCamera = glm::rotate(glm::mat4(1.f), -camRotation.x,
    sideways) * matCamera;
  matCamera = glm::rotate(glm::mat4(1.f), -camRotation.y,
    glm::vec3(0.f, 1.f, 0.f)) * matCamera;

  // Draw the globe

  // Stencil for outline
  // GL_ALWAYS = Never discard the fragment, GL_REPLACE means 1 is
  // put into stencil buffer
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  // Value you put into stencil buffer is ANDed with 0xFF
  glStencilMask(0xFF);
  
  // Placement matrix
  matPlacement = glm::mat4(1.f);

  // Put object into world
  matPlacement = glm::scale(glm::mat4(1.f),
    glm::vec3(1.f, 1.f, 1.f)) * 
    matPlacement;
  
  matPlacement = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(1.f, 0.f, 0.f)) * matPlacement;
  matPlacement = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 1.f, 0.f)) * matPlacement;
  matPlacement = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 0.f, 1.f)) * matPlacement;
  
  // Object position is (0, 0, -2)
  matPlacement = glm::translate(glm::mat4(1.f),
    glm::vec3(0.f, 0.f, -2.f)) * matPlacement;
  
  // Modify object relative to the eye
  matPlacement = matCamera * matPlacement;

  // Bind shader program
  glUseProgram(to_pid);

  // Fill in matrices
  glUniformMatrix4fv(to_perspectiveLoc, 1, GL_FALSE,
    glm::value_ptr(matPerspective));
  glUniformMatrix4fv(to_placementLoc, 1, GL_FALSE,
    glm::value_ptr(matPlacement));

  // Bind vertex array object
  glBindVertexArray(to_vaoID);

  // Bind texture to texture unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sphere_texBufID);
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
  // Value you put into stencil buffer is ANDed with 0xFF
  glStencilMask(0x00);
  // Depth for floor purposes ???
  glDisable(GL_DEPTH_TEST);

  // Placement matrix
  matPlacement = glm::mat4(1.f);

  // Put object into world
  matPlacement = glm::scale(glm::mat4(1.f),
    glm::vec3(1.25f, 1.25f, 1.25f)) * 
    matPlacement;
  
  matPlacement = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(1.f, 0.f, 0.f)) * matPlacement;
  matPlacement = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 1.f, 0.f)) * matPlacement;
  matPlacement = glm::rotate(glm::mat4(1.f), 0.f,
    glm::vec3(0.f, 0.f, 1.f)) * matPlacement;
  
  // Object position is (0, 0, -2)
  matPlacement = glm::translate(glm::mat4(1.f),
    glm::vec3(0.f, 0.f, -2.f)) * matPlacement;
  
  // Modify object relative to the eye
  matPlacement = matCamera * matPlacement;

  // Bind shader program
  glUseProgram(do_pid);

  // Fill in matrices
  glUniformMatrix4fv(do_perspectiveLoc, 1, GL_FALSE,
    glm::value_ptr(matPerspective));
  glUniformMatrix4fv(do_placementLoc, 1, GL_FALSE,
    glm::value_ptr(matPlacement));

  // Bind vertex array object
  glBindVertexArray(do_sphere_vaoID);

  // Draw one object
  glDrawElements(GL_TRIANGLES, bunny_eleBufSize, GL_UNSIGNED_INT,
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

