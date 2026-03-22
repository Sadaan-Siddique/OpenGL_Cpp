// The three general basic things for a window to popup (SDL2 or GLFW): 
// Initialize
// Main Loop 
    // Input
    // Updates
    // Render / PreRender
// Clean up

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using std::cout, std::endl;

// In C++, you can't define a function inside another function
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// We also want to have some form of input control in GLFW and we can achieve this with several of GLFW’s input functions. We’ll be using GLFW’s glfwGetKey function that takes the window as input together with a key. The function returns whether this key is currently being pressed. We’re creating a processInput function to keep all input code organized:
void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}


void GetOpenGLVersionInfo()
{
    cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "Version: " << glGetString(GL_VERSION) << endl;
    cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // checks Are you an Apple Mac computer?
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn_OpenGL", NULL, NULL);
    if(window == NULL)
    {
        cout << "Failed to create GLFW Window." << endl;
        glfwTerminate();
        return -1;  
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialze GLAD" << endl;
        return -1;
    }

    // Rendering is the process of generating a 2D image from a 3D model or description
    // 'to render' commonly means to generate an image or video from a precise description (often created by an artist) using a computer program
    // glViewport(0, 0, 800, 600);
    // render (kuch bnana) -> provide or give (a service), cause to be or become(make)
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // render loop
    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // input
        processInput(window);
        // rendering commands here
        // ...
        // check and call events and swap the buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    GetOpenGLVersionInfo();
    

    glfwTerminate();
    return 0;

}