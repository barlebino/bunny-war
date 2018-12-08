// TODO: Change to bind program -> bind vao -> make mat -> fill mat
// TODO: does sphere have normal data?
// TODO: Placement structs for non-lights

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
#include "shaders_c/shadowdepth_shader.hpp"
#include "shaders_c/blur_shader.hpp"

// Placement struct
// Contains scale, rotation, and translate matrices
struct Placement {
  glm::vec3 scale;
  glm::vec3 rotate; // TODO: Change? Quaternions?
  glm::vec3 translate;
};

// Light types
struct PointLight {
  struct Placement placement;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  // Attenuation
  float constant;
  float linear;
  float quadratic;
};
struct DirectionalLight {
  glm::vec3 direction;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
};

// Model for depth shading
struct DepthModel {
  unsigned vaoID;
  bool indexed;
};

// VAOs for shadow mapping
struct DepthModel sphereDepthModel;
struct DepthModel bunnyDepthModel;
struct DepthModel cubeDepthModel;

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

// Input
char keys[6] = {0, 0, 0, 0, 0, 0};

// Light objects
struct PointLight point_lights[3];
struct DirectionalLight directional_lights[1];

// VAO IDs
unsigned rect_vaoID;
unsigned blur_rect_vaoID;
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
struct ShadowDepthShader sdShader;
struct BlurShader blurShader;

// Height of window ???
int g_width = 1280;
int g_height = 960;

// Framebuffer for postprocessing
unsigned int fbo;
unsigned int fbo_color_texture;
unsigned int fbo_bright_color_texture; // Bloom
unsigned int fbo_depth_stencil_texture;

// Framebuffer with depth only for shadow mapping
unsigned int shadow_fbo;
unsigned int shadow_color_texture; // TESTING
unsigned int shadow_depth_texture;
int shadow_width = 1280;
int shadow_height = 960;

// Ping pong framebuffers for bloom
unsigned int pingpongFBO[2];
unsigned int pingpongBuffer[2];

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

// Debug
bool cameraFreeze = false;

// Object placements
struct Placement phong_bunny_placement = {
  glm::vec3(1.f, 1.f, 1.f), // scale
  glm::vec3(0.f, 0.f, 0.f), // rotate
  glm::vec3(-12.f, 0.f, -2.f) // translate
};
struct Placement wood_cube_placement = {
  glm::vec3(1.f, 1.f, 1.f), // scale
  glm::vec3(0.f, 0.f, 0.f), // rotate
  glm::vec3(-12.f, 0.f, -4.f) // translate
};
struct Placement face_cube_placement = {
  glm::vec3(1.f, 1.f, 1.f), // scale
  glm::vec3(0.f, 0.f, 0.f), // rotate
  glm::vec3(-12.f, 0.f, 4.f) // translate
};
struct Placement phong_globe_placement = {
  glm::vec3(1.f, 1.f, 1.f), // scale
  glm::vec3(0.f, 0.f, 0.f), // rotate
  glm::vec3(-8.f, 0.f, 6.f) // translate
};

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
      printf("rotate: %f\n", face_cube_placement.rotate.y);
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, g_width * ssaaLevel,
    g_height * ssaaLevel, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Attach texture to fbo
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
    fbo_color_texture, 0);
  // Unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  // Bright color texture for fbo, for bloom
  glGenTextures(1, &fbo_bright_color_texture);
  glBindTexture(GL_TEXTURE_2D, fbo_bright_color_texture);
  // Multiply by level for super sample anti-aliasing
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, 
    g_width * ssaaLevel, g_height * ssaaLevel, 0,
    GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Attach texture to fbo
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 
    fbo_bright_color_texture, 0);
  // Unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  // TODO: Remove depth and stencil textures
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

  // TODO: Convert to only depth attachment
  // TESTING COLOR ATTACHMENT
  // Create framebuffer with depth only for shadows
  glGenFramebuffers(1, &shadow_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
  // Depth texture
  glGenTextures(1, &shadow_depth_texture);
  glBindTexture(GL_TEXTURE_2D, shadow_depth_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
    shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
    shadow_depth_texture, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  // No color buffer
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  // Unbind shadow framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Ping pong framebuffers for bloom
  glGenFramebuffers(2, pingpongFBO);
  // Color buffers for ping pong
  glGenTextures(2, pingpongBuffer);
  for(int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, g_width / 2,
        g_height / 2, 0, GL_RGB, GL_FLOAT, NULL
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        pingpongBuffer[i], 0
    );
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    // Unbind framebuffer to normal framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

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
  gammaImageLoad("../resources/world.bmp", &world_texBufID);

  // ------ Load grass texture ------
  gammaImageLoad("../resources/grass.png", &grass_texBufID);

  // -------- Load face cubemaps --------

  // ------ Load diffuse container texture ------
  gammaImageLoad("../resources/woodcube/diffuse_container.png",
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
  gammaCubemapLoad(faces, &skybox_texBufID);

  // ---- Load wood cube diffuse ----
  faces = {
    "../resources/woodcube/diffuse_container.png",
    "../resources/woodcube/diffuse_container.png",
    "../resources/woodcube/diffuse_container.png",
    "../resources/woodcube/diffuse_container.png",
    "../resources/woodcube/diffuse_container.png",
    "../resources/woodcube/diffuse_container.png"
  };
  gammaCubemapLoad(faces, &woodcube_diffuseMapID);

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

  // ------ Shadow Depth Shader ------
  sdShader.pid = initShader(
    "../resources/shaders_glsl/vertShadowDepth.glsl",
    "../resources/shaders_glsl/fragShadowDepth.glsl");

  // Put locations of attribs and uniforms into sdShader
  getShadowDepthShaderLocations(&sdShader);

  // ------ Gaussian blur shader ------
  blurShader.pid = initShader(
    "../resources/shaders_glsl/vertBlur.glsl",
    "../resources/shaders_glsl/fragBlur.glsl");
  getBlurShaderLocations(&blurShader);

  // -------- Initialize VAOS --------
  
  // Texture-only rectangle (screen)
  makeTextureShaderVAO(&rect_vaoID, &textureShader,
    rect_posBufID, rect_texCoordBufID, rect_eleBufID);

  // Ping pong rectangle (Gaussian blur)
  makeBlurShaderVAO(&blur_rect_vaoID, &blurShader,
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

  // ------ VAOs for shadow ------
  
  // Sphere shadow VAO
  makeShadowDepthShaderVAO(&sphereDepthModel.vaoID, &sdShader,
    sphere_posBufID, sphere_eleBufID);
  sphereDepthModel.indexed = true;

  // Bunny shadow VAO
  makeShadowDepthShaderVAO(&bunnyDepthModel.vaoID, &sdShader,
    bunny_posBufID, bunny_eleBufID);
  bunnyDepthModel.indexed = true;

  // Cube shadow VAO
  makeShadowDepthShaderVAO(&cubeDepthModel.vaoID, &sdShader,
    convexbox_posBufID, 0); // Not indexed
  cubeDepthModel.indexed = false;

  // -------- Initialize Lights --------
  Placement tempPlacement;
  
  // ------ Point Light 0 ------
  tempPlacement = {
    glm::vec3(.5f, .5f, .5f), // scale
    glm::vec3(0.f, 0.f, 0.f), // rotate
    glm::vec3(-8.f, 0.f, -2.f) // translate
  };
  point_lights[0] = {
    tempPlacement,
    glm::vec3(0.f, 0.f, .2f), // ambient
    glm::vec3(0.f, 0.f, .5f), // diffuse
    glm::vec3(0.f, 0.f, 1.f), // specular
    1.f, // constant
    .22f, // linear
    .20f // quadratic
  };
  // ------ Point Light 1 ------
  tempPlacement = {
    glm::vec3(.5f, .5f, .5f), // scale
    glm::vec3(0.f, 0.f, 0.f), // rotate
    glm::vec3(-14.f, 0.f, 2.f) // translate
  };
  point_lights[1] = {
    tempPlacement,
    glm::vec3(0.f, .2f, 0.f), // ambient
    glm::vec3(0.f, .5f, 0.f), // diffuse
    glm::vec3(0.f, 1.f, 0.f), // specular
    1.f, // constant
    .22f, // linear
    .20f // quadratic
  };
  // ------ Point Light 2 ------
  tempPlacement = {
    glm::vec3(.5f, .5f, .5f), // scale
    glm::vec3(0.f, 0.f, 0.f), // rotate
    glm::vec3(-8.f, 2.f, 0.f) // translate
  };
  point_lights[2] = {
    tempPlacement,
    glm::vec3(.2f, 0.f, 0.f), // ambient
    glm::vec3(.5f, 0.f, 0.f), // diffuse
    glm::vec3(1.f, 0.f, 0.f), // specular
    1.f, // constant
    .22f, // linear
    .20f // quadratic
  };
  // ------ Directional Light 0 ------
  directional_lights[0] = {
    glm::vec3(-1.f, 0.f, -1.f), // direction
    glm::vec3(1.f, 1.f, 1.f), // ambient
    glm::vec3(2.5f, 2.5f, 2.5f), // diffuse
    glm::vec3(5.f, 5.f, 5.f) // specular
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

// TODO: For only one directional light
static void lightRender() {
  int width, height;
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glEnable(GL_DEPTH_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  // Set permanent matrices
  // TODO: Calculated in render AND lightRender (repetitive!)
  float near_plane = 1.0f, far_plane = 25.f;
  glm::mat4 proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 
    near_plane, far_plane);
  // TODO: change location of direction light based on lookAt
  // TODO: Attach a placement struct to directional light?
  glm::mat4 view = glm::lookAt(
    glm::vec3(-4.f, 0.f, 8.f),
    glm::vec3(-4.f, 0.f, 8.f) + directional_lights[0].direction,
    glm::vec3(0.f, 1.f, 0.f)
  );

  // Temp matrices
  glm::mat4 model;
  glm::mat4 modelviewproj;

  // Render with shadowDepth shader
  glUseProgram(sdShader.pid);

  // TODO: Use rotation and scale in model matrix
  // TODO: Element size / buffer size is per model, not per VAO

  // Render bunny (bunnies?)
  glBindVertexArray(bunnyDepthModel.vaoID);
  // Get first (only?) bunny
  model = glm::mat4(1.f);
  model = glm::rotate(glm::mat4(1.f),
    phong_bunny_placement.rotate.x,
    glm::vec3(1.f, 0.f, 0.f)) * model;
  model = glm::rotate(glm::mat4(1.f),
    phong_bunny_placement.rotate.y,
    glm::vec3(0.f, 1.f, 0.f)) * model;
  model = glm::rotate(glm::mat4(1.f),
    phong_bunny_placement.rotate.z,
    glm::vec3(0.f, 0.f, 1.f)) * model;
  model = glm::translate(glm::mat4(1.f),
    phong_bunny_placement.translate) * model;
  modelviewproj = proj * view * model;
  glUniformMatrix4fv(sdShader.modelviewproj, 1, GL_FALSE,
    glm::value_ptr(modelviewproj));
  // Draw, indexed
  glDrawElements(GL_TRIANGLES, bunny_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);
  // Unbind, done rendering depth bunnies
  glBindVertexArray(0);

  // Render spheres
  glBindVertexArray(sphereDepthModel.vaoID);
  // Get globe
  model = glm::translate(glm::mat4(1.f),
    phong_globe_placement.translate);
  modelviewproj = proj * view * model;
  glUniformMatrix4fv(sdShader.modelviewproj, 1, GL_FALSE,
    glm::value_ptr(modelviewproj));
  // Draw, indexed
  glDrawElements(GL_TRIANGLES, sphere_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);
  // Get point light 0
  model = glm::translate(glm::mat4(1.f),
    point_lights[0].placement.translate);
  modelviewproj = proj * view * model;
  glUniformMatrix4fv(sdShader.modelviewproj, 1, GL_FALSE,
    glm::value_ptr(modelviewproj));
  // Draw, indexed
  glDrawElements(GL_TRIANGLES, sphere_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);
  // Get point light 1
  model = glm::translate(glm::mat4(1.f),
    point_lights[1].placement.translate);
  modelviewproj = proj * view * model;
  glUniformMatrix4fv(sdShader.modelviewproj, 1, GL_FALSE,
    glm::value_ptr(modelviewproj));
  // Draw, indexed
  glDrawElements(GL_TRIANGLES, sphere_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);
  // Get point light 2
  model = glm::translate(glm::mat4(1.f),
    point_lights[2].placement.translate);
  modelviewproj = proj * view * model;
  glUniformMatrix4fv(sdShader.modelviewproj, 1, GL_FALSE,
    glm::value_ptr(modelviewproj));
  // Draw, indexed
  glDrawElements(GL_TRIANGLES, sphere_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);
  // Unbind, done rendering spheres
  glBindVertexArray(0);

  // Render cubes
  glBindVertexArray(cubeDepthModel.vaoID);
  // Get wood cube
  // TODO: Does not take rotation or scale into account
  model = glm::translate(glm::mat4(1.f),
    wood_cube_placement.translate);
  modelviewproj = proj * view * model;
  glUniformMatrix4fv(sdShader.modelviewproj, 1, GL_FALSE,
    glm::value_ptr(modelviewproj));
  // Draw, not indexed
  // Divide by 3 because per vertex
  glDrawArrays(GL_TRIANGLES, 0, convexbox_bufSize / 3);
  // Get face cube
  model = glm::mat4(1.f);
  model = glm::rotate(glm::mat4(1.f),
    face_cube_placement.rotate.x,
    glm::vec3(1.f, 0.f, 0.f)) * model;
  model = glm::rotate(glm::mat4(1.f),
    face_cube_placement.rotate.y,
    glm::vec3(0.f, 1.f, 0.f)) * model;
  model = glm::rotate(glm::mat4(1.f),
    face_cube_placement.rotate.z,
    glm::vec3(0.f, 0.f, 1.f)) * model;
  model = glm::translate(glm::mat4(1.f),
    face_cube_placement.translate) * model;
  modelviewproj = proj * view * model;
  glUniformMatrix4fv(sdShader.modelviewproj, 1, GL_FALSE,
    glm::value_ptr(modelviewproj));
  // Draw, not indexed
  // Divide by 3 because per vertex
  glDrawArrays(GL_TRIANGLES, 0, convexbox_bufSize / 3);
  // Unbind, done rendering cubes
  glBindVertexArray(0);

  // Unbind, done rendering depth objects
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void render() {
  int width, height;
  float aspect;

  // TODO: Update physics
  // Bunny update
  phong_bunny_placement.rotate.y =
    phong_bunny_placement.rotate.y + .01f;
  if(phong_bunny_placement.rotate.y > 6.28f)
    phong_bunny_placement.rotate.y = 0.f;
  // Face cube update
  face_cube_placement.rotate.y =
    face_cube_placement.rotate.y + .01f;
  if(face_cube_placement.rotate.y > 6.28f)
    face_cube_placement.rotate.y = 0.f;

  // Create matrices
  glm::mat4 matModel;
  glm::mat4 matView;
  glm::mat4 matModelview;
  glm::mat4 matProjection;
  // Matrix for directional light
  glm::mat4 matRotation;
  // Matrix for directional light space
  glm::mat4 matLightspace;

  // TODO: Framebuffer size before binding framebuffer???
  // Get current frame buffer size ???
  glfwGetFramebufferSize(window, &width, &height);

  // Draw scene from directional light POV
  lightRender();
  // TODO: calculate light viewproj only once (repetitive!)
  // TODO: dynamic adding of lights + shadows
  // viewproj calculated in render and lightRender
  // TODO: Calculated in render AND lightRender (repetitive!)
  float near_plane = 1.0f, far_plane = 25.f;
  glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 
    near_plane, far_plane);
  // TODO: change location of direction light based on lookAt
  // TODO: Attach a placement struct to directional light?
  glm::mat4 lightView = glm::lookAt(
    glm::vec3(-4.f, 0.f, 8.f),
    glm::vec3(-4.f, 0.f, 8.f) + directional_lights[0].direction,
    glm::vec3(0.f, 1.f, 0.f)
  );

  // Draw the scene from camera POV
  // Change framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  // Tells to what dimensions it renders to
  glViewport(0, 0, width * ssaaLevel, height * ssaaLevel);
  
  // Buffer stuff (???)
  glEnable(GL_DEPTH_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  // Render to two colorbuffers
  // glDrawBuffers is framebuffer state
  unsigned int hdrAttachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, hdrAttachments);
  // Clear framebuffer
  glClearColor(0.f, 0.f, 0.f, 1.f);
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

  // TODO: why skybox affected by glm::lookAt
  matView = glm::rotate(glm::mat4(1.f), -camRotation.x,
    sideways) * matView;
  matView = glm::rotate(glm::mat4(1.f), -camRotation.y,
    glm::vec3(0.f, 1.f, 0.f)) * matView;

  // Rotation matrix
  // TODO: Repetitive matrix
  matRotation = glm::mat4(1.f);
  matRotation = glm::rotate(glm::mat4(1.f), -camRotation.x,
    sideways) * matRotation;
  matRotation = glm::rotate(glm::mat4(1.f), -camRotation.y,
    glm::vec3(0.f, 1.f, 0.f)) * matRotation;

  // Draw the first light source

  // Do nothing to the stencil buffer ever
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  // Placement matrix
  matModel = glm::mat4(1.f);
  
  // Put object into world
  matModel = glm::scale(glm::mat4(1.f),
    point_lights[0].placement.scale) * matModel;
  
  matModel = glm::rotate(glm::mat4(1.f),
    point_lights[0].placement.rotate.x,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    point_lights[0].placement.rotate.y,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    point_lights[0].placement.rotate.z,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  matModel = glm::translate(glm::mat4(1.f),
    point_lights[0].placement.translate) * matModel;

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
    glm::value_ptr(point_lights[0].specular));

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
    point_lights[1].placement.scale) * matModel;
  
  matModel = glm::rotate(glm::mat4(1.f),
    point_lights[1].placement.rotate.x,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    point_lights[1].placement.rotate.y,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    point_lights[1].placement.rotate.z,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  matModel = glm::translate(glm::mat4(1.f),
    point_lights[1].placement.translate) * matModel;

  // Move object relative to the eye
  matModelview = matView * matModel;

  // Fill in uniforms
  glUniformMatrix4fv(ocShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(ocShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  glUniform3fv(ocShader.in_color, 1,
    glm::value_ptr(point_lights[1].specular));

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
    point_lights[2].placement.scale) * matModel;
  
  matModel = glm::rotate(glm::mat4(1.f),
    point_lights[2].placement.rotate.x,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    point_lights[2].placement.rotate.y,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    point_lights[2].placement.rotate.z,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  matModel = glm::translate(glm::mat4(1.f),
    point_lights[2].placement.translate) * matModel;

  // Move object relative to the eye
  matModelview = matView * matModel;

  // Fill in uniforms
  glUniformMatrix4fv(ocShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(ocShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  glUniform3fv(ocShader.in_color, 1,
    glm::value_ptr(point_lights[2].specular));

  // Draw one object
  glDrawElements(GL_TRIANGLES, sphere_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // TODO: Change order of render (VAO, attach uniforms, shader binding...)

  // Draw the phong bunny

  // Do nothing to the stencil buffer ever
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  // Placement matrix
  matModel = glm::mat4(1.f);

  // Put object into world
  matModel = glm::scale(glm::mat4(1.f),
    phong_bunny_placement.scale) * 
    matModel;

  matModel = glm::rotate(glm::mat4(1.f),
    phong_bunny_placement.rotate.x,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    phong_bunny_placement.rotate.y,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    phong_bunny_placement.rotate.z,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  // Object position is defined by placement struct
  matModel = glm::translate(glm::mat4(1.f),
    phong_bunny_placement.translate) * matModel;

  // Directional light space matrix
  matLightspace = lightProj * lightView * matModel;
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
  // Directional light shadow transform
  glUniformMatrix4fv(ompShader.lightspace, 1, GL_FALSE,
    glm::value_ptr(matLightspace));
  // For each point light, input into shader
  // TODO: If same shader, no need to call uniform repeatedly
  // TODO: 3 is a magic number
  for(int i = 0; i < 3; i++) {
    // Give position of light in view space
    // View space transformation
    glUniform3fv(ompShader.pointLights[i].position, 1,
      glm::value_ptr(
        glm::vec3(
          matView * glm::vec4(point_lights[i].placement.translate, 1.f)
        )
      )
    );
    glUniform3fv(ompShader.pointLights[i].ambient, 1,
      glm::value_ptr(point_lights[i].ambient));
    glUniform3fv(ompShader.pointLights[i].diffuse, 1,
      glm::value_ptr(point_lights[i].diffuse));
    glUniform3fv(ompShader.pointLights[i].specular, 1,
      glm::value_ptr(point_lights[i].specular));
    // Attenuation
    // Range of 50, from:
    // http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    glUniform1f(ompShader.pointLights[i].constant, point_lights[i].constant);
    glUniform1f(ompShader.pointLights[i].linear, point_lights[i].linear);
    glUniform1f(ompShader.pointLights[i].quadratic, point_lights[i].quadratic);
  }
  // For each directional light, input into shader
  // TODO: 1 is a magic number
  for(int i = 0; i < 1; i++) {
    // Give direction of directional light in view space
    glUniform3fv(ompShader.directionalLights[i].direction, 1,
      glm::value_ptr(
        glm::vec3(
          matRotation * glm::vec4(directional_lights[i].direction, 1.f)
        )
      )
    );
    glUniform3fv(ompShader.directionalLights[i].ambient, 1,
      glm::value_ptr(directional_lights[i].ambient));
    glUniform3fv(ompShader.directionalLights[i].diffuse, 1,
      glm::value_ptr(directional_lights[i].diffuse));
    glUniform3fv(ompShader.directionalLights[i].specular, 1,
      glm::value_ptr(directional_lights[i].specular));
  }

  // Bind the shadow depth map
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, shadow_depth_texture);
  // 0 because texture unit GL_TEXTURE0
  glUniform1i(ompShader.shadowMap, 0);

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

  // Draw the wood cube

  // Do nothing to the stencil buffer ever
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);

  // Placement matrix
  matModel = glm::mat4(1.f);

  // Put object into world
  matModel = glm::scale(glm::mat4(1.f),
    wood_cube_placement.scale) * 
    matModel;
  
  matModel = glm::rotate(glm::mat4(1.f),
    wood_cube_placement.rotate.x,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    wood_cube_placement.rotate.y,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    wood_cube_placement.rotate.z,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  // Object position is -12, 0, -4
  matModel = glm::translate(glm::mat4(1.f),
    wood_cube_placement.translate) * matModel;

  // Directional light space matrix
  matLightspace = lightProj * lightView * matModel;
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
  // Shininess is 64.0, a MAGIC NUMBER
  glUniform1f(pcShader.materialShininess, 64.f);
  // Directional light shadow transform
  glUniformMatrix4fv(pcShader.lightspace, 1, GL_FALSE,
    glm::value_ptr(matLightspace));
  // For each point light, input into shader
  // TODO: Number of lights is 3
  for(int i = 0; i < 3; i++) {
    // Give light position in view space
    glUniform3fv(pcShader.pointLights[i].position, 1,
      glm::value_ptr(
        glm::vec3(
          matView * glm::vec4(point_lights[i].placement.translate, 1.f)
        )
      )
    );
    glUniform3fv(pcShader.pointLights[i].ambient, 1,
      glm::value_ptr(point_lights[i].ambient));
    glUniform3fv(pcShader.pointLights[i].diffuse, 1,
      glm::value_ptr(point_lights[i].diffuse));
    glUniform3fv(pcShader.pointLights[i].specular, 1,
      glm::value_ptr(point_lights[i].specular));
    // Attenuation
    // Range of 50, from:
    // http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    glUniform1f(pcShader.pointLights[i].constant, point_lights[i].constant);
    glUniform1f(pcShader.pointLights[i].linear, point_lights[i].linear);
    glUniform1f(pcShader.pointLights[i].quadratic, point_lights[i].quadratic);
  }
  // For each directional light, input into shader
  // TODO: 1 is a magic number
  for(int i = 0; i < 1; i++) {
    // Give direction of directional light in view space
    glUniform3fv(pcShader.directionalLights[i].direction, 1,
      glm::value_ptr(
        glm::vec3(
          matRotation * glm::vec4(directional_lights[i].direction, 1.f)
        )
      )
    );
    glUniform3fv(pcShader.directionalLights[i].ambient, 1,
      glm::value_ptr(directional_lights[i].ambient));
    glUniform3fv(pcShader.directionalLights[i].diffuse, 1,
      glm::value_ptr(directional_lights[i].diffuse));
    glUniform3fv(pcShader.directionalLights[i].specular, 1,
      glm::value_ptr(directional_lights[i].specular));
  }

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
  // TODO: 4 shadow maps in one shadow map?
  // Shadow Map
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, shadow_depth_texture);
  glUniform1i(pcShader.shadowMap, 2);

  // Draw the cube
  // Divide by 3 because per vertex
  glDrawArrays(GL_TRIANGLES, 0, convexbox_bufSize / 3);

  // Unbind maps
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, 0);

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
    face_cube_placement.scale) * 
    matModel;
  
  matModel = glm::rotate(glm::mat4(1.f),
    face_cube_placement.rotate.x,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    face_cube_placement.rotate.y,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    face_cube_placement.rotate.z,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  // Object position is -12, 0, -4
  matModel = glm::translate(glm::mat4(1.f),
    face_cube_placement.translate) * matModel;

  // Directional light space matrix
  matLightspace = lightProj * lightView * matModel;
  // Modify object relative to the eye
  matModelview = matView * matModel;

  // Bind shader program
  glUseProgram(ofpcShader.pid);

  // Fill in shader uniforms
  glUniformMatrix4fv(ofpcShader.modelview, 1, GL_FALSE,
    glm::value_ptr(matModelview));
  glUniformMatrix4fv(ofpcShader.projection, 1, GL_FALSE,
    glm::value_ptr(matProjection));
  // Directional light shadow transform
  glUniformMatrix4fv(ofpcShader.lightspace, 1, GL_FALSE,
    glm::value_ptr(matLightspace));
  // For each light, input into shader
  // TODO: Number of lights is 3
  for(int i = 0; i < 3; i++) {
    // Give light position in view space
    glUniform3fv(ofpcShader.pointLights[i].position, 1,
      glm::value_ptr(
        glm::vec3(
          matView * glm::vec4(point_lights[i].placement.translate, 1.f)
        )
      )
    );
    glUniform3fv(ofpcShader.pointLights[i].ambient, 1,
      glm::value_ptr(point_lights[i].ambient));
    glUniform3fv(ofpcShader.pointLights[i].diffuse, 1,
      glm::value_ptr(point_lights[i].diffuse));
    glUniform3fv(ofpcShader.pointLights[i].specular, 1,
      glm::value_ptr(point_lights[i].specular));
    // Attenuation
    glUniform1f(ofpcShader.pointLights[i].constant, point_lights[i].constant);
    glUniform1f(ofpcShader.pointLights[i].linear, point_lights[i].linear);
    glUniform1f(ofpcShader.pointLights[i].quadratic, point_lights[i].quadratic);
  }
  // For each directional light, input into shader
  // TODO: 1 is a magic number
  for(int i = 0; i < 1; i++) {
    // Give direction of directional light in view space
    glUniform3fv(ofpcShader.directionalLights[i].direction, 1,
      glm::value_ptr(
        glm::vec3(
          matRotation * glm::vec4(directional_lights[i].direction, 1.f)
        )
      )
    );
    glUniform3fv(ofpcShader.directionalLights[i].ambient, 1,
      glm::value_ptr(directional_lights[i].ambient));
    glUniform3fv(ofpcShader.directionalLights[i].diffuse, 1,
      glm::value_ptr(directional_lights[i].diffuse));
    glUniform3fv(ofpcShader.directionalLights[i].specular, 1,
      glm::value_ptr(directional_lights[i].specular));
  }

  // Bind vertex array object
  glBindVertexArray(facecube_vaoID);

  // Shininess is 64.0, a MAGIC NUMBER
  glUniform1f(ofpcShader.materialShininess, 64.f);
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
  // Bind shadow depth map
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, shadow_depth_texture);
  glUniform1i(ofpcShader.shadowMap, 2);

  // Draw one object
  glDrawArrays(GL_TRIANGLES, 0, convexbox_bufSize / 3);

  // Unbind maps
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, 0);

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
    phong_globe_placement.scale) * 
    matModel;
  
  matModel = glm::rotate(glm::mat4(1.f),
    phong_globe_placement.rotate.x,
    glm::vec3(1.f, 0.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    phong_globe_placement.rotate.y,
    glm::vec3(0.f, 1.f, 0.f)) * matModel;
  matModel = glm::rotate(glm::mat4(1.f),
    phong_globe_placement.rotate.z,
    glm::vec3(0.f, 0.f, 1.f)) * matModel;

  // Object position is -8, 0, 4
  matModel = glm::translate(glm::mat4(1.f),
    phong_globe_placement.translate) * matModel;

  // Directional light space matrix
  matLightspace = lightProj * lightView * matModel;
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
  // Bind shadow depth map
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, shadow_depth_texture);
  glUniform1i(phongShader.shadowMap, 1);
  // Directional light shadow transform
  glUniformMatrix4fv(phongShader.lightspace, 1, GL_FALSE,
    glm::value_ptr(matLightspace));
  // For each point light, input into shader
  // TODO: If same shader, no need to call uniform repeatedly
  for(int i = 0; i < 3; i++) {
    glUniform3fv(phongShader.pointLights[i].position, 1,
      glm::value_ptr(
        glm::vec3(
          matView * glm::vec4(point_lights[i].placement.translate, 1.f)
        )
      )
    );
    glUniform3fv(phongShader.pointLights[i].ambient, 1,
      glm::value_ptr(point_lights[i].ambient));
    glUniform3fv(phongShader.pointLights[i].diffuse, 1,
      glm::value_ptr(point_lights[i].diffuse));
    glUniform3fv(phongShader.pointLights[i].specular, 1,
      glm::value_ptr(point_lights[i].specular));
    // Attenuation
    glUniform1f(phongShader.pointLights[i].constant,
      point_lights[i].constant);
    glUniform1f(phongShader.pointLights[i].linear,
      point_lights[i].linear);
    glUniform1f(phongShader.pointLights[i].quadratic,
      point_lights[i].quadratic);
  }
  // For each directional light, input into shader
  // TODO: 1 is a magic number
  for(int i = 0; i < 1; i++) {
    // Give direction of directional light in view space
    glUniform3fv(phongShader.directionalLights[i].direction, 1,
      glm::value_ptr(
        glm::vec3(
          matRotation * glm::vec4(directional_lights[i].direction, 1.f)
        )
      )
    );
    glUniform3fv(phongShader.directionalLights[i].ambient, 1,
      glm::value_ptr(directional_lights[i].ambient));
    glUniform3fv(phongShader.directionalLights[i].diffuse, 1,
      glm::value_ptr(directional_lights[i].diffuse));
    glUniform3fv(phongShader.directionalLights[i].specular, 1,
      glm::value_ptr(directional_lights[i].specular));
  }

  // Draw one object
  glDrawElements(GL_TRIANGLES, sphere_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE1);
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
  
  // Perform Guassian blur on color attachment
  int numBlurs = 6;
  glDisable(GL_DEPTH_TEST);
  // Stencil
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0x00);
  // Shader program
  glUseProgram(blurShader.pid);
  // VAO
  glBindVertexArray(blur_rect_vaoID);
  // Only uses one texture
  glActiveTexture(GL_TEXTURE0);
  for(int i = 0; i < numBlurs; i++) {
    // Blur horizontally
    // Copy color buffer to ping pong color buffer #0
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i % 2]);
    //glViewport(0, 0, width * ssaaLevel, height * ssaaLevel);
    glViewport(0, 0, width / 2, height / 2);
    // Uniform for type of blur
    glUniform1i(blurShader.horizontal, i % 2);
    // TODO: Initial coniditons, copy operation before for loop?
    if(i == 0) {
      // TODO: change to blur the bright texture
      glBindTexture(GL_TEXTURE_2D, fbo_bright_color_texture);
    } else {
      glBindTexture(GL_TEXTURE_2D, pingpongBuffer[(i + 1) % 2]);
    }
    glUniform1i(blurShader.texLoc, 0);
    // Draw
    glDrawElements(GL_TRIANGLES, rect_eleBufSize, GL_UNSIGNED_INT,
      (const void *) 0);
  }
  // Unbind
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  glUseProgram(0);

  // Paste from side framebuffer to default framebuffer

  // Bind normal framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, width, height);
  // Enable gamma correction
  glEnable(GL_FRAMEBUFFER_SRGB);
  // Clear
  glClearColor(0.f, 0.f, 0.f, 1.f);
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

  // Fill in shader uniforms
  // TODO: Automatic exposure adjustment
  glUniform1f(textureShader.exposure, 1.f);
  // Bind textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fbo_color_texture);
  glUniform1i(textureShader.texLoc, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
  glUniform1i(textureShader.bloomBuffer, 1);

  // Draw screen
  glDrawElements(GL_TRIANGLES, rect_eleBufSize, GL_UNSIGNED_INT,
    (const void *) 0);

  // Unbind texture
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Unbind VAO
  glBindVertexArray(0);

  // Unbind shader program
  glUseProgram(0);

  // Re-enable
  glStencilMask(0xFF);
  glEnable(GL_DEPTH_TEST);

  // Disable gamma correction
  glDisable(GL_FRAMEBUFFER_SRGB);
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

  // Face culling
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
    // TODO: Add FPS counter
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

