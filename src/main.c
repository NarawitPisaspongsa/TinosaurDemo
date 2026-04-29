#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <ecs/ecs.h>
#include <game/components.h>
#include <systems/camera_system.h>
#include <systems/render_system.h>
#include <systems/input_system.h>

/* --- Input Globals (Initialized to Zero) --- */
static float g_mouse_dx = 0.0f;
static float g_mouse_dy = 0.0f;
static int first_mouse  = 1;
static float last_x     = 1280.0f / 2.0f;
static float last_y     = 720.0f  / 2.0f;

/* --- Embedded Shader Sources --- */
static const char *VERT_SRC =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "}\n";

static const char *FRAG_SRC =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec3 objectColor;\n"
    "void main() {\n"
    "    FragColor = vec4(objectColor, 1.0);\n"
    "}\n";

/* --- Cube Vertex Data --- */
static const float CUBE_VERTS[] = {
    -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
     0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
    -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f
};

// Prototypes
void init_gl_and_window(GLFWwindow** window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void sync_input(GLFWwindow* window, Registry* reg, Entity player);
GLuint setup_cube_vao();
GLuint create_demo_shader(const char* vert_src, const char* frag_src);

int main() {
    printf("--- Engine Starting ---\n");
    fflush(stdout);

    GLFWwindow* window = NULL;
    init_gl_and_window(&window);

    /* 1. Allocate Registry on Heap */
    Registry* registry = (Registry*)malloc(sizeof(Registry));
    if (!registry) return -1;
    ecs_init(registry);

    /* 2. Register Components */
    ecs_register_component(registry, COMP_TRANSFORM, sizeof(TransformComponent));
    ecs_register_component(registry, COMP_CAMERA,    sizeof(CameraComponent));
    ecs_register_component(registry, COMP_INPUT,     sizeof(InputComponent));

    /* 3. Visuals Init */
    GLuint shader = create_demo_shader(VERT_SRC, FRAG_SRC);
    GLuint VAO    = setup_cube_vao();

    /* 4. Create Player Entity */
    Entity player = ecs_create_entity(registry);
    
    TransformComponent* t = (TransformComponent*)ecs_add_component(registry, player, COMP_TRANSFORM);
    t->position[0] = 0.0f; t->position[1] = 0.0f; t->position[2] = 3.0f;
    t->yaw   = -90.0f;
    t->pitch =  0.0f;

    CameraComponent* c = (CameraComponent*)ecs_add_component(registry, player, COMP_CAMERA);
    vec3 up_init = {0.0f, 1.0f, 0.0f};
    glm_vec3_copy(up_init, c->world_up);
    c->fov = 45.0f;

    ecs_add_component(registry, player, COMP_INPUT);

    printf("--- ECS World Ready ---\n");
    fflush(stdout);

    /* 5. Main Loop */
    float last_frame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float current_frame = (float)glfwGetTime();
        float delta_time = current_frame - last_frame;
        last_frame = current_frame;

        glfwPollEvents();

        // Layer 0: Bridge OS/Input to ECS
        sync_input(window, registry, player);


        // LAYER 0: Input (Reads Hardware -> Writes to InputComponent)
        input_system_update(registry, window);

        // Layer 1: Logic
        camera_system_update(registry, delta_time);

        // RESET MOUSE DELTAS FOR NEXT FRAME
        g_mouse_dx = 0.0f;
        g_mouse_dy = 0.0f;

        // Layer 12: Render
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        render_system_update(registry, shader, VAO, player);

        glfwSwapBuffers(window);
    }

    free(registry);
    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    if (first_mouse) {
        last_x = (float)xpos;
        last_y = (float)ypos;
        first_mouse = 0;
    }

    g_mouse_dx = (float)xpos - last_x;
    g_mouse_dy = last_y - (float)ypos; // Reversed: y-coords range from bottom to top

    last_x = (float)xpos;
    last_y = (float)ypos;
}

void sync_input(GLFWwindow* window, Registry* reg, Entity player) {
    InputComponent* input = (InputComponent*)ecs_get_component(reg, player, COMP_INPUT);
    if (!input) return;

    // Movement axes
    input->y_axis = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) input->y_axis += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) input->y_axis -= 1.0f;
    
    input->x_axis = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) input->x_axis += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) input->x_axis -= 1.0f;

    // Mouse deltas
    input->mouse_dx = g_mouse_dx;
    input->mouse_dy = g_mouse_dy;
}

void init_gl_and_window(GLFWwindow** window) {
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    *window = glfwCreateWindow(1280, 720, "ECS Camera Demo", NULL, NULL);
    if (!*window) { glfwTerminate(); exit(EXIT_FAILURE); }

    glfwMakeContextCurrent(*window);
    
    // CAPTURE MOUSE
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(*window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) exit(EXIT_FAILURE);
    
    glEnable(GL_DEPTH_TEST);
}

GLuint setup_cube_vao() {
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTS), CUBE_VERTS, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    return VAO;
}

GLuint create_demo_shader(const char* v_src, const char* f_src) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &v_src, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &f_src, NULL);
    glCompileShader(fs);
    GLuint p = glCreateProgram();
    glAttachShader(p, vs); glAttachShader(p, fs); glLinkProgram(p);
    glDeleteShader(vs); glDeleteShader(fs);
    return p;
}
