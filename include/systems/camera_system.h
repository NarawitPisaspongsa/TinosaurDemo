#include <math.h>
#include <stdlib.h>
#include <cglm/cglm.h>
#include <ecs/ecs.h>
#include <game/components.h>

/**
 * CAMERA SYSTEM (Logic Layer)
 * ---------------------------
 * Reads: InputComponent (Axes, deltas, and action flags)
 * Reads/Writes: TransformComponent (Logical world position/Euler angles)
 * Writes: CameraComponent (Orientation vectors and final View Matrix)
 */
void camera_system_update(Registry* reg, float delta_time) {
    Signature mask = (1ULL << COMP_TRANSFORM) | (1ULL << COMP_CAMERA) | (1ULL << COMP_INPUT);

    // Keep track of time for the sine wave oscillation
    static float bob_time = 0.0f;

    for (Entity e = 0; e < reg->entity_count; e++) {
        if ((reg->signatures[e] & mask) == mask) {
            
            TransformComponent* transform = (TransformComponent*)ecs_get_component(reg, e, COMP_TRANSFORM);
            CameraComponent* camera       = (CameraComponent*)ecs_get_component(reg, e, COMP_CAMERA);
            InputComponent* input         = (InputComponent*)ecs_get_component(reg, e, COMP_INPUT);

            // 1. Mouse Look
            transform->yaw   += input->mouse_dx * 0.1f;
            transform->pitch += input->mouse_dy * 0.1f;
            if (transform->pitch > 89.0f)  transform->pitch = 89.0f;
            if (transform->pitch < -89.0f) transform->pitch = -89.0f;
            transform->yaw = fmodf(transform->yaw, 360.0f);

            // 2. Speed and Zoom Logic
            float base_speed = input->is_running ? 10.0f : 5.0f; 
            float current_speed = base_speed * delta_time;

            if (input->is_zooming) {
                camera->fov = 15.0f; 
            } else {
                camera->fov = 45.0f; 
            }

            // 3. Basis Vectors
            vec3 new_front;
            new_front[0] = cosf(glm_rad(transform->yaw)) * cosf(glm_rad(transform->pitch));
            new_front[1] = sinf(glm_rad(transform->pitch));
            new_front[2] = sinf(glm_rad(transform->yaw)) * cosf(glm_rad(transform->pitch));
            glm_vec3_normalize_to(new_front, camera->front);

            glm_vec3_crossn(camera->front, camera->world_up, camera->right);
            glm_vec3_crossn(camera->right, camera->front, camera->up);

            // 4. Movement
            vec3 move_delta = {0.0f, 0.0f, 0.0f};
            vec3 flat_front = { camera->front[0], 0.0f, camera->front[2] };
            glm_vec3_normalize(flat_front);

            if (input->y_axis != 0.0f) {
                glm_vec3_muladds(flat_front, input->y_axis * current_speed, move_delta);
            }
            if (input->x_axis != 0.0f) {
                glm_vec3_muladds(camera->right, input->x_axis * current_speed, move_delta);
            }
            
            glm_vec3_add(transform->position, move_delta, transform->position);

            // 5. SHAKE & BOB CALCULATION
            vec3 visual_offset = {0.0f, 0.0f, 0.0f};

            // A. Gunfire Jitter (Random)
            if (input->is_firing) {
                float jitter = 0.05f;
                visual_offset[0] += ((float)rand()/(float)RAND_MAX * 2.0f - 1.0f) * jitter;
                visual_offset[1] += ((float)rand()/(float)RAND_MAX * 2.0f - 1.0f) * jitter;
            }

            // B. Headbob (Sine wave oscillation while moving)
            // Check if the player is actually moving
            float move_mag = fabsf(input->x_axis) + fabsf(input->y_axis);
            if (move_mag > 0.1f) {
                // Increase oscillation speed if running
                float bob_speed = input->is_running ? 12.0f : 8.0f;
                float bob_amount = input->is_running ? 0.15f : 0.06f;
                
                bob_time += delta_time * bob_speed;
                
                // Vertical bob (Up/Down)
                visual_offset[1] += sinf(bob_time) * bob_amount;
                // Horizontal sway (Side to Side)
                visual_offset[0] += cosf(bob_time * 0.5f) * (bob_amount * 0.5f);
            } else {
                // Smoothly reset bob_time when standing still to avoid "snapping"
                bob_time = glm_lerp(bob_time, 0.0f, delta_time * 5.0f);
            }

            // 6. Final View Matrix
            vec3 eye_final;
            vec3 center_final;

            glm_vec3_add(transform->position, visual_offset, eye_final);
            glm_vec3_add(eye_final, camera->front, center_final);

            glm_lookat(eye_final, center_final, camera->up, camera->view_matrix);
        }
    }
}
