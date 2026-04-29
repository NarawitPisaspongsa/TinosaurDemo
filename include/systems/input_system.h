#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <GLFW/glfw3.h>
#include <ecs/ecs.h>

/**
 * INPUT SYSTEM (Layer 0)
 * ---------------------
 * Purpose: Decouples OS/Hardware events from game logic.
 * Reads: GLFW Keyboard/Mouse state.
 * Writes: InputComponent data for any entity with the COMP_INPUT signature.
 */
void input_system_update(Registry* reg, GLFWwindow* window);

#endif
