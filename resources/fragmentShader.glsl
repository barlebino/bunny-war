#version 330 core

uniform sampler2D texCol;

in vec2 vert_texCoord;
in vec3 vert_col;

out vec4 color;

// Taken from https://learnopengl.com/Advanced-OpenGL/Depth-testing
float near = 0.1; 
float far  = 10.0; 

float LinearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0; // back to NDC 
  return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {
  //color = texture(texCol, vert_texCoord);
  //color = vec4(1.0, 0.0, 0.0, 1.0);
  float depth = LinearizeDepth(gl_FragCoord.z) / far;
  // divide by far for demonstration
  color = vec4(vec3(depth), 1.0);
}
