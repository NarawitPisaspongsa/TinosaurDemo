cmake_minimum_required(VERSION 3.15)
project(windows C)

# Use standard C11
set(CMAKE_C_STANDARD 11)

# CRITICAL FOR NEOVIM: Generate compile_commands.json for Clangd LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- 1. SETUP VENDORED GLFW ---
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")

# Add the local GLFW folder to our build
add_subdirectory(vendor/glfw-3.4)

# --- 2. SETUP OUR GAME EXECUTABLE ---
add_executable(main 
    src/main.c
    src/glad.c
    src/utils/shader_load.c
)

# --- 3. INCLUDE DIRECTORIES ---
# This tells the compiler where to look for <cglm/cglm.h>
# and your local project headers.
target_include_directories(main PRIVATE
    include        # This should contain the 'cglm' folder
    src/utils
    vendor/glfw-3.4/include # Ensure GLFW headers are visible
)

# --- 4. LINK EVERYTHING TOGETHER ---
target_link_libraries(main PRIVATE glfw opengl32)
