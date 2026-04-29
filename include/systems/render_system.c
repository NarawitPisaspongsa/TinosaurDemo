#include "systems/render_system.h"
#include "game/components.h"
#include <cglm/cglm.h>

void render_system_update(Registry* reg, GLuint shader, GLuint VAO, Entity player_camera) {
    // 1. Retrieve Camera and Transform Components
    CameraComponent* cam = (CameraComponent*)ecs_get_component(reg, player_camera, COMP_CAMERA);
    TransformComponent* trans = (TransformComponent*)ecs_get_component(reg, player_camera, COMP_TRANSFORM);

    if (!cam || !trans) return;

    // 2. Prepare Shader
    glUseProgram(shader);

    // 3. Set Global Uniforms (View & Projection)
    GLint loc_view = glGetUniformLocation(shader, "view");
    GLint loc_proj = glGetUniformLocation(shader, "projection");
    
    // Use the view_matrix recomputed in Layer 1 (Camera Layer)
    glUniformMatrix4fv(loc_view, 1, GL_FALSE, (float*)cam->view_matrix);

    // Build Projection based on current scroll-zoom FOV
    mat4 projection;
    float aspect = 1280.0f / 720.0f; 
    glm_perspective(glm_rad(cam->fov), aspect, 0.1f, 100.0f, projection);
    glUniformMatrix4fv(loc_proj, 1, GL_FALSE, (float*)projection);

    // 4. Bind Geometry
    glBindVertexArray(VAO);
    GLint loc_model = glGetUniformLocation(shader, "model");
    GLint loc_color = glGetUniformLocation(shader, "objectColor");

    // 5. Draw Scene Objects
    struct { vec3 pos; vec3 col; float s; } targets[] = {
        {{ 3.0f, 0.0f, -4.0f}, {1.0f, 0.2f, 0.2f}, 1.0f}, // Red
        {{-3.0f, 0.0f, -6.0f}, {0.2f, 0.9f, 0.3f}, 1.3f}, // Green
        {{ 0.0f, 0.0f,-10.0f}, {0.2f, 0.5f, 1.0f}, 2.0f}  // Blue
    };

    for (int i = 0; i < 3; i++) {
        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, targets[i].pos);
        glm_scale_uni(model, targets[i].s);
        
        glUniformMatrix4fv(loc_model, 1, GL_FALSE, (float*)model);
        glUniform3fv(loc_color, 1, targets[i].col);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}
