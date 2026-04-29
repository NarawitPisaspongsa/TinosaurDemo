cmake_minimum_required(VERSION 3.15)
project(windows C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- 1. SETUP VENDORED GLFW ---
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")

add_subdirectory(vendor/glfw-3.4)

# --- 2. SETUP OUR GAME EXECUTABLE ---
# ADD src/ecs/ecs.c HERE so the linker can find your ECS functions!
add_executable(main
    src/glad.c
    src/main.c
    include/ecs/ecs.c
    include/systems/render_system.c
    include/systems/input_system.c
    src/utils/shader_load.c
)

# --- 3. INCLUDE DIRECTORIES ---
target_include_directories(main PRIVATE
    include
    src/utils
    vendor/glfw-3.4/include
)

# --- 4. LINK EVERYTHING TOGETHER ---
target_link_libraries(main PRIVATE glfw opengl32)
