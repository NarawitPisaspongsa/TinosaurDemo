#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>
#include "utils/shader_load.h"

void framebuffer_size_callback(GLFWwindow*, int, int);
void processInput(GLFWwindow*);

typedef struct {
  float r, g, b, a;
} colorVal;

void clearcolor_struct(colorVal);

// for vertex shader, we use const char* global variable (see fragmentShaderSource)
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

#ifdef _WIN32
#include <windows.h>
#include <libloaderapi.h>
#endif

// ... inside main() ...


int main() {

#ifdef _WIN32
  char path[MAX_PATH];
  // Get the full path of the executable
  GetModuleFileNameA(NULL, path, MAX_PATH);

  // Find the last backslash to isolate the directory
  char* lastBackslash = strrchr(path, '\\');
  if (lastBackslash) {
    *lastBackslash = '\0'; // Terminate the string at the slash
    SetCurrentDirectoryA(path); // Change the working directory to the exe folder
  }
#endif

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

  if (window == NULL){
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to init GLAD\n");
    return -1;
  }

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  colorVal red = {1.0f, 0.0f, 0.0f, 1.0f};
   
  float vertices[] = {
    0.5f, 0.5f, 0.0f, // top right
    0.5f, -0.5f, 0.0f, // bottom right
    // -0.5f, 0.5f, 0.0f, // top left

    // 0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f // top left
  };

  unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3 // second triangle
  };


  // store vertexShader as int, returned as a reference id
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  // compile the shader
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // see compile success or not
  int success_vs; char infoLog_vs[512];

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success_vs);

  if(!success_vs) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog_vs);
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog_vs); 
  }

  // getting the fragment shader source (using a file instead)
  char * fragmentShaderSource = get_shader_content("../src/shaders/orange_tri.fs");

  if (fragmentShaderSource == NULL || strlen(fragmentShaderSource) == 0) {
      printf("CRITICAL ERROR: Shader source is empty or file not found!\n");
  } else {
      printf("Fragment Shader Loaded:\n%s\n", fragmentShaderSource);
  }
  // compile fragment shader
  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, (const char**)&fragmentShaderSource, NULL);

  glCompileShader(fragmentShader);

  // see compile success or not
  int success_fs; char infoLog_fs[512];

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success_fs);

  if(!success_fs) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog_fs);
    printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog_fs); 
  }

  // getting the ID for shader program
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  // linking the shaders together. Note that in this part, if the output from one shader into another does not match, it will cause linking error
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  free(fragmentShaderSource);

  // see linking and program creation error log
  int success_link; char infoLog_link[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success_link);
  if(!success_link) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog_link);
    printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog_link);
  }

  
  // shader cleanup
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glGenBuffers(1, &VBO);                                        // could also be GL_STREAM_DRAW (data set once, used a few times)
  glBindBuffer(GL_ARRAY_BUFFER, VBO);                           // v          or GL_DYNAMIC_DRAW (data changed a lot, used many times)
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // configuring how vertices should be handles
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // handling element buffer object
  unsigned int EBO;
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // set wireframe mode
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // render loop
  while(!glfwWindowShouldClose(window)) {

    // input processing
    processInput(window);


    clearcolor_struct(red);
    glClear(GL_COLOR_BUFFER_BIT);

    // activating the shader program
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    // check for events
    glfwSwapBuffers(window);
    glfwPollEvents(); // swap buffer -> front & back
  }
  glfwTerminate();

  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}
void clearcolor_struct(colorVal color) {
  glClearColor(color.r, color.g, color.b, color.a);
}
