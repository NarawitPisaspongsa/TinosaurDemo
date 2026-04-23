#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>

void framebuffer_size_callback(GLFWwindow*, int, int);
void processInput(GLFWwindow*);

typedef struct {
  float r, g, b, a;
} colorVal;

void clearcolor_struct(colorVal);

int main(){
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

  // render loop
  while(!glfwWindowShouldClose(window)) {

    // input processing
    processInput(window);


    clearcolor_struct(red);
    glClear(GL_COLOR_BUFFER_BIT);

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
