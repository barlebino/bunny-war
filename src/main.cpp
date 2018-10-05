// TODO: Change to bind program -> bind vao -> make mat -> fill mat
// TODO: does sphere have normal data?

#include <iostream>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define PI 3.14159

#include <unistd.h>

#include "asset_loaders/mesh_load.hpp"
#include "asset_loaders/image_load.hpp"

#include "material.hpp"

#include "shaders_c/onematerialphong_shader.hpp"
#include "shaders_c/onecolor_shader.hpp"
#include "shaders_c/skybox_shader.hpp"
#include "shaders_c/rect_shader.hpp"
#include "shaders_c/depth_shader.hpp"
#include "shaders_c/texture_shader.hpp"
#include "shaders_c/phongcube_shader.hpp"
#include "shaders_c/onefacephongcube_shader.hpp"
#include "shaders_c/phong_shader.hpp"

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

// Light objects
struct Light lights[3];

// Input
char keys[6] = {0, 0, 0, 0, 0, 0};

// VAO IDs
unsigned rect_vaoID;
unsigned skybox_vaoID;
unsigned ls_vaoID;
unsigned omp_bunny_vaoID;
unsigned woodcube_vaoID;
unsigned facecube_vaoID;
unsigned phongglobe_vaoID;

// Sphere data
unsigned sphere_posBufID;
unsigned sphere_norBufID;
unsigned sphere_eleBufID;
unsigned sphere_texCoordBufID;
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
int rect_eleBufSize;

// Phong box data
unsigned convexbox_posBufID;
unsigned convexbox_norBufID;
int convexbox_bufSize;

// Skybox data
unsigned skybox_posBufID;
int skybox_posBufSize;

// Shader programs
// TODO: All shaders inherit from "shader"
struct OneMaterialPhongShader ompShader;
struct OneColorShader ocShader;
struct SkyboxShader sbShader;
struct DepthShader depthShader;
struct TextureShader textureShader;
struct PhongCubeShader pcShader;
struct OneFacePhongCubeShader ofpcShader;
struct PhongShader phongShader;

// Height of window ???
int g_width = 1280;
int g_height = 960;

// Framebuffer for postprocessing
unsigned int fbo;
unsigned int fbo_color_texture;
unsigned int fbo_depth_stencil_texture;

// World texture
unsigned world_texBufID;
// Grass texture
unsigned grass_texBufID;
// Cubemaps
unsigned int skybox_texBufID;
unsigned int woodcube_diffuseMapID;
unsigned int woodcube_specularMapID;
unsigned int facecube_diffuseMapID;
unsigned int facecube_specularMapID;

bool cameraFreeze = false;

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
  } else if(key == GLFW_KEY_P) { // toggle whether or not camera stays in place
    if(action == GLFW_RELEASE) {
      cameraFreeze = !cameraFreeze;
    }
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

  // -------- Initialize all of the meshes --------

  // Sphere mesh
  getMesh("../resources/objs/sphere.obj");
  resizeMesh();
  // Send mesh to GPU and store buffer IDs
  sendMesh(&sphere_posBufID, &sphere_eleBufID, &sphere_texCoordBufID,
    &sphere_eleBufSize, &sphere_norBufID);

  // Bunny mesh
  getMesh("../resources/objs/bunny.obj");
  resizeMesh();
  sendMesh(&bunny_posBufID, &bunny_eleBufID, NULL,
    &bunny_eleBufSize, &bunny_norBufID);
  
  // Rectangle mesh
  getRectangleMesh();
  // No need to resize, explicitly specified coordinates
  sendMesh(&rect_posBufID, &rect_eleBufID, &rect_texCoordBufID,
    &rect_eleBufSize, NULL);

  // Skybox mesh
  getSkyboxMesh();
  sendSkyboxMesh(&skybox_posBufID, &skybox_posBufSize);

  // Phong box mesh
  getPhongBoxMesh();
  sendPhongBoxMesh(&convexbox_posBufID, &convexbox_norBufID,
    &convexbox_bufSize);

  // -------- Load textures onto the GPU --------

  // ------ Load world texture ------
  defaultImageLoad("../resources/world.bmp", &world_texBufID); 

  // ------ Load grass texture ------
  defaultImageLoad("../resources/grass.png", &grass_texBufID);

  // -------- Load face cubemaps --------

  // ------ Load diffuse container texture ------
  defaultImageLoad("../resources/woodcube/diffuse_container.png",
    &facecube_diffuseMapID);

  // ------ Load specular container texture ------
  defaultImageLoad("../resources/woodcube/specular_container.png",
    &facecube_specularMapID);

  // ------ Load the cubemaps ------
  std::vector<std::string> faces;

  // ---- Load skybox cubemap ----
  faces = {
    "../resources/skybox/right.jpg",
    "../resources/skybox/left.jpg",
    "../resources/skybox/top.jpg",
    "../resources/skybox/bottom.jpg",
    "../resources/skybox/front.jpg",
    "../resources/skybox/back.jpg"
  };
  defaultCubemapLoad(faces, &skybox_texBufID);

  // ---- Load wood cube diffuse ----
  faces = {
    "../resources/woodcube/diffuse_container.png",
    "../resources/woodcube/diffuse_container.png",
    "../resources/woodcube/diffuse_container.png",
    "../resources/woodcube/diffuse_container.png",
    "../resources/woodcube/diffuse_container.png",
    "../resources/woodcube/diffuse_container.png"
  };
  defaultCubemapLoad(faces, &woodcube_diffuseMapID);

  // ---- Load wood cube specular ----
  faces = {
    "../resources/woodcube/specular_container.png",
    "../resources/woodcube/specular_container.png",
    "../resources/woodcube/specular_container.png",
    "../resources/woodcube/specular_container.png",
    "../resources/woodcube/specular_container.png",
    "../resources/woodcube/specular_container.png"
  };
  defaultCubemapLoad(faces, &woodcube_specularMapID);

  // -------- Initialize shader programs --------
  // TODO: each makeVAO assumes a certain data format

  // ------ Texture only shader program ------
  textureShader.pid = initShader(
    "../resources/shaders_glsl/vertTextureOnly.glsl",
    "../resources/shaders_glsl/fragTextureOnly.glsl");

  // Put locations of attribs and uniforms into textureShader
  getTextureShaderLocations(&textureShader);

  // ------ Depth Only shader program ------
  depthShader.pid = initShader(
    "../resources/shaders_glsl/vertDepthOnly.glsl",
    "../resources/shaders_glsl/fragDepthOnly.glsl");

  // Put locations of attribs and uniforms into depthShader
  getDepthShaderLocations(&depthShader);

  // ------ Cubemap shader program ------
  sbShader.pid = initShader(
    "../resources/shaders_glsl/vertSkybox.glsl",
    "../resources/shaders_glsl/fragSkybox.glsl");

  // Put locations of attribs and uniforms into sbShader
  getSkyboxShaderLocations(&sbShader);

  // ------ One color shader program ------
  ocShader.pid = initShader(
    "../resources/shaders_glsl/vertOneColor.glsl",
    "../resources/shaders_glsl/fragOneColor.glsl");

  // Put locations of attribs and uniforms into ocShader
  getOneColorShaderLocations(&ocShader);

  // ------ One material phong shader program ------
  ompShader.pid = initShader(
    "../resources/shaders_glsl/vertOneMaterialPhong.glsl",
    "../resources/shaders_glsl/fragOneMaterialPhong.glsl");

  // Put locations of attribs and uniforms into ompShader
  getOneMaterialPhongShaderLocations(&ompShader);

  // ------ Phong cubemap shader ------
  pcShader.pid = initShader(
    "../resources/shaders_glsl/vertPhongCube.glsl",
    "../resources/shaders_glsl/fragPhongCube.glsl");

  // Put locations of attribs and uniforms into pcShader
  getPhongCubeShaderLocations(&pcShader);

  // ------ One face phong cube shader program ------
  ofpcShader.pid = initShader(
    "../resources/shaders_glsl/vertOneFacePhongCube.glsl",
    "../resources/shaders_glsl/fragOneFacePhongCube.glsl");
  
  // Put locations of attribs and uniforms into ofpcShader
  getOneFacePhongCubeShaderLocations(&ofpcShader);

  // ------ Phong shader, diffuse only ------
  phongShader.pid = initShader(
    "../resources/shaders_glsl/vertPhong.glsl",
    "../resources/shaders_glsl/fragPhong.glsl");

  // Put locations of attribs and uniforms into phongShader
  getPhongShaderLocations(&phongShader);

  // -------- Initialize VAOS --------
  
  // Texture-only rectangle (screen)
  makeTextureShaderVAO(&rect_vaoID, &textureShader,
    rect_posBufID, rect_texCoordBufID, rect_eleBufID);

  // Skybox VAO (sky)
  makeSkyboxShaderVAO(&skybox_vaoID, &sbShader, skybox_posBufID);

  // One color sphere (light source)
  makeOneColorShaderVAO(&ls_vaoID, &ocShader,
    sphere_posBufID, sphere_eleBufID);

  // One material phong bunny (copper bunny)
  makeOneMaterialPhongShaderVAO(&omp_bunny_vaoID, &ompShader,
    bunny_posBufID, bunny_norBufID, bunny_eleBufID);

  // Cubemap phong cube (wooden cube)
  makePhongCubeShaderVAO(&woodcube_vaoID, &pcShader,
    convexbox_posBufID, convexbox_norBufID);

  // Cubemap phong cube (wooden cube)
  makeOneFacePhongCubeShaderVAO(&facecube_vaoID, &ofpcShader,
    convexbox_posBufID, convexbox_norBufID);

  // Textured phong sphere (phong globe)
  makePhongShaderVAO(&phongglobe_vaoID, &phongShader,
    sphere_posBufID, sphere_norBufID, sphere_texCoordBufID,
    sphere_eleBufID);

  // -------- Initialize Lights --------
  lights[0] = {
    glm::vec3(-8.f, 0.f, -2.f), // position
    glm::vec3(.2f, .2f, .2f), // ambient
    glm::vec3(.5f, .5f, .5f), // diffuse
    glm::vec3(1.f, 1.f, 1.f) // specular
  };
  lights[1] = {
    glm::vec3(-14.f, 0.f, 2.f), // position
    glm::vec3(.2f, .2f, .2f), // ambient
    glm::vec3(.5f, .5f, .5f), // diffuse
    glm::vec3(1.f, 1.f, 1.f) // specular
  };
  lights[2] = {
    glm::vec3(-8.f, 2.f, 0.f), // position
    glm::vec3(.2f, .2f, .2f), // ambient
    glm::vec3(.5f, .5f, .5f), // diffuse
    glm::vec3(1.f, 1.f, 1.f) // specular
  };
}

static void handleInput(int width, int height) {
  glm::dvec2 cursorPos;
  glm::dvec2 screenPos;

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
}

static void render() {
  int width, height;
  float aspect;

  // Create matrices
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

  if(!cameraFreeze) {
    handleInput(width, height);
  }

  // Projection matrix
  aspect = width / (float) height;
  matProjection = glm::perspective(70.f, aspect, .1f, 100.f);

  // View matrix
  matView = glm::mat4(1.f);
  matView = glm::translate(glm::mat4(1.f), -camLocation) *
    matView;

  matView = glm::rotate(glm::mat4(1.f), -camRotation.x,
    sideways) * matView;
  matView = glm::rotate(glm::mat4(1.f), -camRotation.y,
    glm::vec3(0.f, 1.f, 0.f)) * matView;

  // Draw the first light source

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
    lights[0].position) * matModel;

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
    glm::value_ptr(lights[0].specular));

  // Bind vertex array object
  glBindVertexArray(ls_vaoID);

  // Draw one object
  glDrawElements(GL_TRIANGLES, sphere_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Draw the second light source

  // Do nothing to the stencil buffer ever
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  // Bind shader program
  glUseProgram(ocShader.pid);

  // Bind vertex array object
  glBindVertexArray(ls_vaoID);

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
    secondLight.position) * matModel;

  // Move object relative to the eye
  matModelview = matView * matModel;

  // Fill in uniforms
  glUniformMatrix4fv(ocShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(ocShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  glUniform3fv(ocShader.in_color, 1,
    glm::value_ptr(lights[0].specular));

  // Draw one object
  glDrawElements(GL_TRIANGLES, sphere_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Draw the third light source

  // Do nothing to the stencil buffer ever
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  // Bind shader program
  glUseProgram(ocShader.pid);

  // Bind vertex array object
  glBindVertexArray(ls_vaoID);

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

  // Object position is (-8, 2, -2)
  matModel = glm::translate(glm::mat4(1.f),
    thirdLight.position) * matModel;

  // Move object relative to the eye
  matModelview = matView * matModel;

  // Fill in uniforms
  glUniformMatrix4fv(ocShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(ocShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  glUniform3fv(ocShader.in_color, 1,
    glm::value_ptr(lights[0].specular));

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

  // Set modelview matrix
  matModelview = matView * matModel;

  // Bind shader program
  glUseProgram(ompShader.pid);

  // Bind vertex array object
  glBindVertexArray(omp_bunny_vaoID);

  // Fill in matrices
  // Fill in vertex shader uniforms
  glUniformMatrix4fv(ompShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(ompShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  // Fill in fragment shader uniforms
  glUniform3fv(ompShader.materialAmbient, 1,
    glm::value_ptr(copper.ambient));
  glUniform3fv(ompShader.materialDiffuse, 1,
    glm::value_ptr(copper.diffuse));
  glUniform3fv(ompShader.materialSpecular, 1,
    glm::value_ptr(copper.specular));
  glUniform1f(ompShader.materialShininess,
    copper.shininess);
  glUniform3fv(ompShader.ompLights[0].position, 1,
    glm::value_ptr(
      glm::vec3(
        matView * glm::vec4(lights[0].position, 1.f)
      )
    )
  );
  glUniform3fv(ompShader.ompLights[0].ambient, 1,
    glm::value_ptr(lights[0].ambient));
  glUniform3fv(ompShader.ompLights[0].diffuse, 1,
    glm::value_ptr(lights[0].diffuse));
  glUniform3fv(ompShader.ompLights[0].specular, 1,
    glm::value_ptr(lights[0].specular));
  // Attenuation
  // Range of 50, from:
  // http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
  glUniform1f(ompShader.ompLights[0].constant, 1.f);
  glUniform1f(ompShader.ompLights[0].linear, .045f);
  glUniform1f(ompShader.ompLights[0].quadratic, .0075f);

  // Draw one object
  glDrawElements(GL_TRIANGLES, bunny_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Draw the wood cube

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

  // Object position is -12, 0, -4
  matModel = glm::translate(glm::mat4(1.f),
    glm::vec3(-12.f, 0.f, -4.f)) * matModel;

  // Modify object relative to the eye
  matModelview = matView * matModel;

  // Bind shader program
  glUseProgram(pcShader.pid);

  // Fill in matrices
  // Fill in vertex shader uniforms
  glUniformMatrix4fv(pcShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(pcShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  // Fill in fragment shader uniforms
  // Give light position in view space
  glUniform3fv(pcShader.lightPosition, 1,
    glm::value_ptr(
      glm::vec3(
        matView * glm::vec4(lights[0].position, 1.f)
      )
    )
  );
  glUniform3fv(pcShader.lightAmbient, 1,
    glm::value_ptr(lights[0].ambient));
  glUniform3fv(pcShader.lightDiffuse, 1,
    glm::value_ptr(lights[0].diffuse));
  glUniform3fv(pcShader.lightSpecular, 1,
    glm::value_ptr(lights[0].specular));
  // Shininess is 64.0, a MAGIC NUMBER
  glUniform1f(pcShader.materialShininess, 64.f);
  // Attenuation
  // Range of 50, from:
  // http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
  glUniform1f(pcShader.lightConstant, 1.f);
  glUniform1f(pcShader.lightLinear, .045f);
  glUniform1f(pcShader.lightQuadratic, .0075f);

  // Bind vertex array object
  glBindVertexArray(woodcube_vaoID);

  // Bind the maps
  // Diffuse Map
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, woodcube_diffuseMapID);
  glUniform1i(pcShader.materialDiffuse, 0);
  // Specular Map
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, woodcube_specularMapID);
  glUniform1i(pcShader.materialSpecular, 1);

  // Draw the cube
  // Divide by 3 because per vertex
  glDrawArrays(GL_TRIANGLES, 0, convexbox_bufSize / 3);

  // Unbind maps
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Draw the other wood cube

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

  // Object position is -12, 0, -4
  matModel = glm::translate(glm::mat4(1.f),
    glm::vec3(-12.f, 0.f, 4.f)) * matModel;

  // Modify object relative to the eye
  matModelview = matView * matModel;

  // Bind shader program
  glUseProgram(ofpcShader.pid);

  // Fill in shader uniforms
  glUniformMatrix4fv(ofpcShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(ofpcShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  // Give light position in view space
  glUniform3fv(ofpcShader.lightPosition, 1,
    glm::value_ptr(
      glm::vec3(
        matView * glm::vec4(lights[0].position, 1.f)
      )
    )
  );
  glUniform3fv(ofpcShader.lightAmbient, 1,
    glm::value_ptr(lights[0].ambient));
  glUniform3fv(ofpcShader.lightDiffuse, 1,
    glm::value_ptr(lights[0].diffuse));
  glUniform3fv(ofpcShader.lightSpecular, 1,
    glm::value_ptr(lights[0].specular));
  // Shininess is 64.0, a MAGIC NUMBER
  glUniform1f(ofpcShader.materialShininess, 64.f);
  // Attenuation
  glUniform1f(ofpcShader.lightConstant, 1.f);
  glUniform1f(ofpcShader.lightLinear, .045f);
  glUniform1f(ofpcShader.lightQuadratic, .0075f);

  // Bind vertex array object
  glBindVertexArray(facecube_vaoID);

  // Bind the maps
  // Bind diffuse map
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, facecube_diffuseMapID);
  // 0 because texture unit GL_TEXTURE0
  glUniform1i(ofpcShader.materialDiffuse, 0);
  // Bind specular map
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, facecube_specularMapID);
  // 1 because texture unit GL_TEXTURE1
  glUniform1i(ofpcShader.materialSpecular, 1);

  // Draw one object
  glDrawArrays(GL_TRIANGLES, 0, convexbox_bufSize / 3);

  // Unbind maps
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Draw the phong globe
  
  // Do nothing to the stencil buffer ever
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  // Bind shader program
  glUseProgram(phongShader.pid);

  // Bind the VAO
  glBindVertexArray(phongglobe_vaoID);

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

  // Object position is -8, 0, 4
  matModel = glm::translate(glm::mat4(1.f),
    glm::vec3(-8.f, 0.f, 4)) * matModel;

  // Modify object relative to the eye
  matModelview = matView * matModel;

  // Fill uniforms
  glUniformMatrix4fv(phongShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(phongShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  // Bind diffuse map
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, world_texBufID);
  glUniform1i(phongShader.materialDiffuse, 0);
  // Fill more uniforms
  glUniform3fv(phongShader.lightPosition, 1,
    glm::value_ptr(
      glm::vec3(
        matView * glm::vec4(lights[0].position, 1.f)
      )
    )
  );
  glUniform3fv(phongShader.lightAmbient, 1,
    glm::value_ptr(lights[0].ambient));
  glUniform3fv(phongShader.lightDiffuse, 1,
    glm::value_ptr(lights[0].diffuse));
  glUniform3fv(phongShader.lightSpecular, 1,
    glm::value_ptr(lights[0].specular));
  // Attenuation
  glUniform1f(phongShader.lightConstant, 1.f);
  glUniform1f(phongShader.lightLinear, .045f);
  glUniform1f(phongShader.lightQuadratic, .0075f);

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

  // Draw the skybox

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
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texBufID);
  // 0 because correct skybox is in texture unit GL_TEXTURE0
  glUniform1i(sbShader.skybox, 0); // Changed

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
  glUseProgram(textureShader.pid);

  // Bind vertex array object
  glBindVertexArray(rect_vaoID);

  // Calculate placement matrix (do nothing)
  matModel = glm::mat4(1.f);

  // Fill in shader uniforms
  glUniformMatrix4fv(textureShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModel));
  glUniformMatrix4fv(textureShader.projection, 1, GL_FALSE,
    glm::value_ptr(matModel));
  // Bind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fbo_color_texture);
  glUniform1i(textureShader.texLoc, 0);

  // Draw screen
  glDrawElements(GL_TRIANGLES, rect_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Unbind VAO
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Re-enable
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

