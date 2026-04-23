cmake_minimum_required(VERSION 3.15)
project(windows C)

# Use standard C11
set(CMAKE_C_STANDARD 11)

# CRITICAL FOR NEOVIM: Generate compile_commands.json for Clangd LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- 1. SETUP VENDORED GLFW ---
# We don't need GLFW's tutorials or tests, so turn them off to compile faster set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")

# Add the local GLFW folder to our build
add_subdirectory(vendor/glfw-3.4)

# --- 2. SETUP OUR GAME EXECUTABLE ---
add_executable(main 
    src/main.c
    src/glad.c
)

# Tell our executable where to find GLAD headers
target_include_directories(main PRIVATE include)

# --- 3. LINK EVERYTHING TOGETHER ---
# Link the locally built GLFW and the standard Windows OpenGL library
target_link_libraries(main PRIVATE glfw opengl32)
