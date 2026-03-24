#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath> // Needed for fmod (range reduction)
using std::cout, std::endl;

// Global Variables
#define PI 3.141592653589793238462649
const unsigned int g_SCR_WIDTH = 800; 
const unsigned int g_SCR_HEIGHT = 800; 
GLFWwindow* g_window;
unsigned int g_VAO;
unsigned int g_EBO;
unsigned int g_VBO;
unsigned int g_vertexShader;
unsigned int g_fragmentShader;
unsigned int g_shaderProgram;

// Shaders
const char* g_vertexShaderSource = R"( 
    #version 330 core
    layout (location = 0) in vec3 aPos; 
    out vec4 vertexColor; // specify a color output to the fragment shader
    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // output variable to dark-red
    }
)";
// We have managed to send a value from the vertex shader to the fragment shader by using out as keyword in vertex shader
const char* g_fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n" // The fragment shader only requires one output variable and that is a vector of size 4 that defines the final color output that we should calculate ourselves.
    // "in vec4 vertexColor;\n" // input variable from vs (same name and type)
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
        // These colors are colors that are used in rasterization
        // "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" // last value in parameter is to define how opaque our rendering object should be
        // "FragColor = vertexColor;\n"
        "FragColor = ourColor;\n"
    "}\0";
// We declared a uniform vec4 ourColor in the fragment shader and set the fragment’s output color to the content of this uniform value. Since uniforms are global variables, we can define them in any shader stage we’d like so no need to go through the vertex shader again to get something to the fragment shader. We’re not using this uniform in the vertex shader so there’s no need to define it there.

// Functions
void windowInitialize();
void vertexSpecification();
void createGraphicsPipeline();
void mainRenderingLoop();
void GetOpenGLVersionInfo();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
long double exponentialFunc(double base, unsigned int exponent);
unsigned long long factorialFunc(unsigned int n);
long double my_sin(double x);

// Main Start
int main()
{
    windowInitialize();

    vertexSpecification();

    createGraphicsPipeline();

    mainRenderingLoop();

    glfwTerminate(); // clean up

    return 0;
}

// Main End

void windowInitialize()
{
    // Initialize Program
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    g_window = glfwCreateWindow(g_SCR_WIDTH, g_SCR_HEIGHT, "GLFW OpenGL Window", NULL, NULL);
    if(g_window == NULL)
    {
        cout << "Failed to create GLFW Window." << endl;
        glfwTerminate();
        // return -1;  
        return;
    }

    glfwMakeContextCurrent(g_window);
    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        // return -1;
        return;
    }
}

void vertexSpecification()
{
    const float vertices[] = {
        // x    y     z
        -0.5f, -0.5f, 0.0f, // vertex 0
        -0.5f, 0.5f, 0.0f, // vertex 1
        0.5f, 0.5f, 0.0f, // vertex 2
        0.5f, -0.5f, 0.0f // vertex 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);

    glGenBuffers(1, &g_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
}

void checkCompilation(const unsigned int shader)
{
    // check for shader compile errors
    int success;
    char infoLog[512];
    if (shader == g_vertexShader || shader == g_fragmentShader)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // Asking OpenGL for the status
        if(!success) // can also be written as : if(success == 0)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl; 
        }
    }
    else if (shader == g_shaderProgram)
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }

}

// void createShaderProgram(const std::string &vertexShaderSource, const std::string &fragmentShaderSource)
void createShaderProgram()
{

    g_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(g_vertexShader, 1, &g_vertexShaderSource, NULL);
    glCompileShader(g_vertexShader);
    checkCompilation(g_vertexShader);

    g_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_fragmentShader, 1, &g_fragmentShaderSource, NULL);
    glCompileShader(g_fragmentShader);
    checkCompilation(g_fragmentShader);

    g_shaderProgram = glCreateProgram();
    glAttachShader(g_shaderProgram, g_vertexShader);
    glAttachShader(g_shaderProgram, g_fragmentShader);
    glLinkProgram(g_shaderProgram);
    checkCompilation(g_shaderProgram);

    glDeleteShader(g_vertexShader);
    glDeleteShader(g_fragmentShader);

}

void createGraphicsPipeline()
{
    createShaderProgram();
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void GetOpenGLVersionInfo()
{
    cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "Version: " << glGetString(GL_VERSION) << endl;
    cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

void mainRenderingLoop()
{
    while(!glfwWindowShouldClose(g_window))
    {
        processInput(g_window);
        
        // Pre Draw
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glClearColor(1.f, 1.f, 0.f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Without this, Framebuffer may behave weirdly.

        glUseProgram(g_shaderProgram); // to actviate the shader

        // update the uniform shader
        // update the uniform color
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        int vertexColorLocation = glGetUniformLocation(g_shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // Draw
        glBindVertexArray(g_VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    GetOpenGLVersionInfo();

    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
    glDeleteProgram(g_shaderProgram);

}

long double exponentialFunc(double base, unsigned int exponent)
{
    if (base == 0 && exponent == 0)
        return -1;

    long double power = 1;
    for(int i = 1; i <= exponent; i++)
        power *= base;
    return power;
}

unsigned long long factorialFunc(unsigned int n)
{
    unsigned long long factorial = 1;
    for(int i = n; i > 0; i--)
        factorial *=  i;
    return factorial;
}

long double my_sin(double x)
{
    x = fmod(x, 2.0 * PI);
    long double sinx = 0;
    // Summation Function of taylor series of sinx
    for (int n = 0; n < 9; n++)
      sinx += ( exponentialFunc(-1, n) * exponentialFunc(x, ((2*n) + 1)) ) / factorialFunc((2*n) + 1);
    
    return sinx;
}