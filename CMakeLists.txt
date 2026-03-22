# In CMake, you must use add_executable to create the program first, and then attach the directories and libraries to it immediately underneath.

cmake_minimum_required(VERSION 4.2.3)

# The Project Name
project(MyOpenGLProject) 

# Tells CMake we are using modern C++
set(CMAKE_CXX_STANDARD 17)
# Find the system libraries we installed via pacman
find_package(OpenGL REQUIRED)



# --- 1. FIRST PROGRAM ---
# Tell CMake the name of our executable and the source file it comes from
add_executable(helloWindow helloWindow.cpp glad.c)
# Tell CMake to look in your current folder for the GLAD includes
target_include_directories(practhelloWindowice PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(helloWindow glfw ${OPENGL_LIBRARIES})
# Link GLFW, GLEW, and OpenGL directly to your executable
# target_link_libraries(helloWindow glfw GLEW ${OPENGL_LIBRARIES})

# --- 2. Fifth PROGRAM ---
add_executable(triangle triangle.cpp glad.c)
# target_link_libraries(triangle glfw GLEW ${OPENGL_LIBRARIES})
target_include_directories(triangle PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(triangle glfw ${OPENGL_LIBRARIES})

# --- 3. Sixth PROGRAM ---
add_executable(EBO EBO.cpp glad.c)
# target_link_libraries(EBO glfw GLEW ${OPENGL_LIBRARIES})
target_include_directories(EBO PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(EBO glfw ${OPENGL_LIBRARIES})