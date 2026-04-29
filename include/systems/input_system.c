#include <systems/input_system.h>
#include <game/components.h>
#include <stdbool.h>

// Internal state to track mouse deltas
static double last_x = 1280.0 / 2.0;
static double last_y = 720.0 / 2.0;
static bool first_mouse = true;

void input_system_update(Registry* reg, GLFWwindow* window) {
    Signature mask = (1ULL << COMP_INPUT);

    // 1. Capture Mouse Delta
    double cur_x, cur_y;
    glfwGetCursorPos(window, &cur_x, &cur_y);
    
    if (first_mouse) {
        last_x = cur_x; 
        last_y = cur_y;
        first_mouse = false;
    }

    float dx = (float)(cur_x - last_x);
    float dy = (float)(last_y - cur_y); // Standard Y inversion
    last_x = cur_x; 
    last_y = cur_y;

    for (Entity e = 0; e < reg->entity_count; e++) {
        if ((reg->signatures[e] & mask) == mask) {
            InputComponent* input = (InputComponent*)ecs_get_component(reg, e, COMP_INPUT);
            if (!input) continue;

            // 2. Movement Axes (W/S/A/D)
            input->x_axis = 0.0f;
            input->y_axis = 0.0f;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) input->y_axis += 1.0f;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) input->y_axis -= 1.0f;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) input->x_axis += 1.0f;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) input->x_axis -= 1.0f;

            // 3. Mouse Deltas for Camera Look
            input->mouse_dx = dx;
            input->mouse_dy = dy;

            // 4. Action Flags
            input->is_jumping = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);      // Space Bar
            input->is_running = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS); // Shift
            input->is_zooming = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS); // Right Click
            input->is_firing  = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);  // Left Click
        }
    }
}
