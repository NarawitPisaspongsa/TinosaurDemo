#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

/* -----------------------------------------------------------------------
 * camera.h  –  Generalized FPS / Driving Camera
 * -----------------------------------------------------------------------
 * Coordinate convention (OpenGL right-hand, Y-up):
 *   +X = right   +Y = up   -Z = forward (into screen)
 *
 * Usage pattern (one frame):
 *   camera_process_keyboard(&cam, dir, deltaTime);
 *   camera_process_mouse(&cam, xoffset, yoffset);
 *   camera_get_view_matrix(&cam, view);
 * ----------------------------------------------------------------------- */

/* ── Tunable defaults ── */
#define CAMERA_DEFAULT_YAW         -90.0f   /* face -Z on startup            */
#define CAMERA_DEFAULT_PITCH          0.0f
#define CAMERA_DEFAULT_SPEED          5.0f   /* world-units per second        */
#define CAMERA_DEFAULT_SENSITIVITY    0.1f   /* degrees per pixel             */
#define CAMERA_DEFAULT_FOV           45.0f   /* degrees                       */
#define CAMERA_PITCH_LIMIT           89.0f   /* clamp to avoid gimbal flip    */

/* ── Movement directions (passed to camera_process_keyboard) ── */
typedef enum {
    CAM_FORWARD  = 0,
    CAM_BACKWARD = 1,
    CAM_LEFT     = 2,
    CAM_RIGHT    = 3
} CameraDirection;

/* ── Camera mode ── */
typedef enum {
    CAM_MODE_FPS     = 0,   /* movement locked to XZ plane (no flying)  */
    CAM_MODE_FREE    = 1    /* full 6-DOF fly-through                   */
} CameraMode;

/* ── Main camera struct ── */
typedef struct {
    /* World-space position & orientation */
    vec3  position;     /* camera origin in world space                 */
    vec3  front;        /* unit vector pointing where we look           */
    vec3  up;           /* camera-local up (recomputed each frame)      */
    vec3  right;        /* camera-local right                           */
    vec3  world_up;     /* reference up – usually (0,1,0)               */

    /* Euler angles (degrees) */
    float yaw;          /* rotation around Y – left/right look          */
    float pitch;        /* rotation around X – up/down look             */

    /* Behaviour */
    float speed;        /* movement speed (world-units / second)        */
    float sensitivity;  /* mouse sensitivity (degrees / pixel)          */
    float fov;          /* field of view in degrees                     */
    CameraMode mode;    /* FPS (ground-locked) or FREE (fly-through)    */
} Camera;

/* -----------------------------------------------------------------------
 * camera_init
 *   position  – starting position in world space
 *   world_up  – global up vector, pass NULL for default (0,1,0)
 *   yaw       – initial yaw in degrees (use CAMERA_DEFAULT_YAW)
 *   pitch     – initial pitch in degrees
 * ----------------------------------------------------------------------- */
static inline void camera_init(Camera *cam,
                                vec3 position,
                                vec3 world_up,
                                float yaw,
                                float pitch,
                                CameraMode mode)
{
    glm_vec3_copy(position, cam->position);

    if (world_up)
        glm_vec3_copy(world_up, cam->world_up);
    else
        glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, cam->world_up);

    cam->yaw         = yaw;
    cam->pitch       = pitch;
    cam->speed       = CAMERA_DEFAULT_SPEED;
    cam->sensitivity = CAMERA_DEFAULT_SENSITIVITY;
    cam->fov         = CAMERA_DEFAULT_FOV;
    cam->mode        = mode;

    /* Derive front/right/up from the initial angles */
    vec3 front;
    front[0] = cosf(glm_rad(cam->yaw))   * cosf(glm_rad(cam->pitch));
    front[1] = sinf(glm_rad(cam->pitch));
    front[2] = sinf(glm_rad(cam->yaw))   * cosf(glm_rad(cam->pitch));
    glm_vec3_normalize_to(front, cam->front);
    glm_vec3_crossn(cam->front, cam->world_up, cam->right);
    glm_vec3_crossn(cam->right, cam->front,    cam->up);
}

/* -----------------------------------------------------------------------
 * camera_update_vectors  (internal – call after changing yaw/pitch)
 * ----------------------------------------------------------------------- */
static inline void camera_update_vectors(Camera *cam)
{
    vec3 front;
    front[0] = cosf(glm_rad(cam->yaw))   * cosf(glm_rad(cam->pitch));
    front[1] = sinf(glm_rad(cam->pitch));
    front[2] = sinf(glm_rad(cam->yaw))   * cosf(glm_rad(cam->pitch));
    glm_vec3_normalize_to(front, cam->front);

    glm_vec3_crossn(cam->front, cam->world_up, cam->right);
    glm_vec3_crossn(cam->right, cam->front,    cam->up);
}

/* -----------------------------------------------------------------------
 * camera_get_view_matrix
 *   Fills `dest` (mat4) with the LookAt view matrix.
 * ----------------------------------------------------------------------- */
static inline void camera_get_view_matrix(const Camera *cam, mat4 dest)
{
    vec3 center;
    glm_vec3_add((float *)cam->position, (float *)cam->front, center);
    glm_lookat((float *)cam->position, center, (float *)cam->up, dest);
}

/* -----------------------------------------------------------------------
 * camera_process_keyboard
 *   dir       – one of the CameraDirection enum values
 *   delta_time – seconds since last frame (for frame-rate independent speed)
 * ----------------------------------------------------------------------- */
static inline void camera_process_keyboard(Camera *cam,
                                            CameraDirection dir,
                                            float delta_time)
{
    float velocity = cam->speed * delta_time;

    if (cam->mode == CAM_MODE_FPS) {
        /* Lock movement to the XZ plane: zero-out the Y component of front */
        vec3 flat_front = { cam->front[0], 0.0f, cam->front[2] };
        glm_vec3_normalize(flat_front);

        vec3 delta = {0};
        if (dir == CAM_FORWARD)  glm_vec3_scale(flat_front,  velocity, delta);
        if (dir == CAM_BACKWARD) glm_vec3_scale(flat_front, -velocity, delta);
        if (dir == CAM_RIGHT)    glm_vec3_scale(cam->right,   velocity, delta);
        if (dir == CAM_LEFT)     glm_vec3_scale(cam->right,  -velocity, delta);
        glm_vec3_add(cam->position, delta, cam->position);

    } else {
        /* Free / fly-through mode – move along actual front vector */
        vec3 delta = {0};
        if (dir == CAM_FORWARD)  glm_vec3_scale(cam->front,   velocity, delta);
        if (dir == CAM_BACKWARD) glm_vec3_scale(cam->front,  -velocity, delta);
        if (dir == CAM_RIGHT)    glm_vec3_scale(cam->right,   velocity, delta);
        if (dir == CAM_LEFT)     glm_vec3_scale(cam->right,  -velocity, delta);
        glm_vec3_add(cam->position, delta, cam->position);
    }
}

/* -----------------------------------------------------------------------
 * camera_process_mouse
 *   xoffset / yoffset – pixel deltas since last frame
 *   constrain_pitch   – pass 1 (true) to clamp pitch (recommended for FPS)
 * ----------------------------------------------------------------------- */
static inline void camera_process_mouse(Camera *cam,
                                         float xoffset,
                                         float yoffset,
                                         int   constrain_pitch)
{
    cam->yaw   += xoffset * cam->sensitivity;
    cam->pitch += yoffset * cam->sensitivity;   /* note: caller decides sign */

    if (constrain_pitch) {
        if (cam->pitch >  CAMERA_PITCH_LIMIT) cam->pitch =  CAMERA_PITCH_LIMIT;
        if (cam->pitch < -CAMERA_PITCH_LIMIT) cam->pitch = -CAMERA_PITCH_LIMIT;
    }

    /* Wrap yaw so it doesn't grow without bound */
    if (cam->yaw >  360.0f) cam->yaw -= 360.0f;
    if (cam->yaw < -360.0f) cam->yaw += 360.0f;

    camera_update_vectors(cam);
}

/* -----------------------------------------------------------------------
 * camera_process_scroll
 *   Adjusts FOV for a zoom effect.  Caller passes this into glm_perspective.
 * ----------------------------------------------------------------------- */
static inline void camera_process_scroll(Camera *cam, float yoffset)
{
    cam->fov -= yoffset;
    if (cam->fov <  1.0f)  cam->fov =  1.0f;
    if (cam->fov > 90.0f)  cam->fov = 90.0f;
}

/* -----------------------------------------------------------------------
 * camera_set_speed / camera_set_sensitivity
 *   Convenience setters (can also write fields directly).
 * ----------------------------------------------------------------------- */
static inline void camera_set_speed(Camera *cam, float speed)
            { cam->speed = speed; }
static inline void camera_set_sensitivity(Camera *cam, float sensitivity)
            { cam->sensitivity = sensitivity; }

#endif /* CAMERA_H */
