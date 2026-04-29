#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <glad/glad.h>
#include "../ecs/ecs.h"

/**
 * Executes Layer 12 of the architecture.
 * Reads: Transform and Camera components.
 * Writes: GPU Commands only.
 */
void render_system_update(Registry* reg, GLuint shader, GLuint VAO, Entity player_camera);

#endif
