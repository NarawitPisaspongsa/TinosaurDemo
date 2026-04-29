#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <cglm/cglm.h>

// Component IDs (for our bitmask)
#define COMP_TRANSFORM 0
#define COMP_CAMERA    1
#define COMP_INPUT     2

typedef struct {
    vec3 position;
    float yaw;
    float pitch;
} TransformComponent;

typedef struct {
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;
    float fov;
    int mode; // 0 = FPS, 1 = Free
    mat4 view_matrix;
    mat4 proj_matrix;
} CameraComponent;

#define COMP_TRANSFORM 0
#define COMP_CAMERA    1
#define COMP_INPUT     2

typedef struct {
    float x_axis;     // A/D (Strafe)
    float y_axis;     // W/S (Forward/Back)
    float mouse_dx;   // Horizontal look
    float mouse_dy;   // Vertical look
    
    // Action Flags
    bool is_jumping;  // Space
    bool is_running;  // Shift
    bool is_zooming;  // Right Click
    bool is_firing;   // Left Click
} InputComponent;
#endif
