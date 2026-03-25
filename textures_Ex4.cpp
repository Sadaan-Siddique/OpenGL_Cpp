#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include "mySin.h"
using std::cout, std::endl;

// Global Variables
const unsigned int g_SCR_WIDTH = 800; 
const unsigned int g_SCR_HEIGHT = 800; 
GLFWwindow* g_window;
unsigned int g_VAO, g_EBO, g_VBO, g_texture1, g_texture2, g_vertexShader, g_fragmentShader, g_shaderProgram;
float g_mixValue = 0.5f;

// Shaders
const char* g_vertexShaderSource = R"( 
    #version 330 core
    layout (location = 0) in vec3 aPos; // position has attribute position 0
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec2 aTexCoord; 

    out vec3 ourColor;
    out vec2 TexCoord;

    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        ourColor = aColor;
        TexCoord = aTexCoord;
    }
)";

const char* g_fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"

    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"

    "uniform sampler2D texture1;\n" // The first image
    "uniform sampler2D texture2;\n" // The second image
    "uniform float mixValue;\n" // will receive the data from C++, and then plug the variable directly into the mix function

    "void main()\n"
    "{\n"
        // Mix the two images together!
       "FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue);\n"
    "}\0";

// Functions
void windowInitialize();
void vertexSpecification();
void createGraphicsPipeline();
void createTextures();
void mainRenderingLoop();
void GetOpenGLVersionInfo();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Main Start
int main()
{
    cout << std::filesystem::current_path() << endl;
    windowInitialize();

    vertexSpecification();

    createGraphicsPipeline();

    createTextures();

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
        exit(-1); // Instantly kills the program
    }

    glfwMakeContextCurrent(g_window);
    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        // return -1;
        exit(-1); // Instantly kills the program
    }
}

void vertexSpecification()
{
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   4.0f, 4.0f, // top right (300%, 300%)
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   4.0f, 0.0f, // bottom right (300%, 0%)
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left (0%, 0%)
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 4.0f  // top left (0%, 300%)
    };

    // float vertices[] = {
    //     // positions // colors // texture coords
    //     0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    //     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    //     -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
    // };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);

    glGenBuffers(1, &g_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color Attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1); 

    // Texture Attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
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

void createTextures()
{
    // ==========================================
    // TEXTURE 1 SETUP
    // ==========================================

    // Creating Textures
    glGenTextures(1, &g_texture1);
    glBindTexture(GL_TEXTURE_2D, g_texture1);

    // Settign Texture Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Loading image
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("../images/me_nicoll_Pencil_ZUFjQg.jpg", &width, &height, &nrChannels, 0);


    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;
        // Tell OpenGL to use 1-byte alignment to prevent segfaults with RGB images
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture 1\n";
        if (!data) 
        {
            cout << "Error: " << stbi_failure_reason() << endl;
        }
        return;
    }   
    stbi_image_free(data);

    // ==========================================
    // TEXTURE 2 SETUP
    // ==========================================

    glGenTextures(1, &g_texture2);
    glBindTexture(GL_TEXTURE_2D, g_texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("../images/blue-bmp-16-bit.bmp", &width, &height, &nrChannels, 0);


    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture 2\n";
        if (!data) 
        {
            cout << "Error: " << stbi_failure_reason() << endl;
        }
        return;
    }   
    stbi_image_free(data);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Will increase the mix() value when pressing up
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        g_mixValue += 0.005f; // Change this number to make it fade faster or slower
        if(g_mixValue >= 1.0f)
            g_mixValue = 1.0f; // will Lock it at 1.0 maximum
    }

    // Will decrease the mix() value when pressing up
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        g_mixValue -= 0.005f; // Change this number to make it fade faster or slower
        if(g_mixValue <= 0.0f)
            g_mixValue = 0.0f; // will Lock it at 0.0 maximum
    }

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

        // Activate texture unit

        // for texture 1
        glActiveTexture(GL_TEXTURE0);
        // Put the first image record on it
        glBindTexture(GL_TEXTURE_2D, g_texture1);
        // Tell the shader variable "texture1" to read from Texture Unit 0
        glUniform1i(glGetUniformLocation(g_shaderProgram, "texture1"), 0);

        // for texture 2
        glActiveTexture(GL_TEXTURE1);
        // Put the second image record on it
        glBindTexture(GL_TEXTURE_2D, g_texture2);
        // Tell the shader variable "texture2" to read from Texture Unit 1
        glUniform1i(glGetUniformLocation(g_shaderProgram, "texture2"), 1);

        static int i = 0;
        if(i > 20)
        {
            float time = glfwGetTime();
            g_mixValue = my_sin(time); 
            cout << g_mixValue << endl;
            i = 0;
        }
        i++;
        glUniform1f(glGetUniformLocation(g_shaderProgram, "mixValue"), g_mixValue); // Send the real-time C++ float to the GLSL uniform

        // Draw
        // glBindVertexArray(g_VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindTexture(GL_TEXTURE_2D, g_texture1);
        glBindTexture(GL_TEXTURE_2D, g_texture2);

        glBindVertexArray(g_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    GetOpenGLVersionInfo();

    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
    glDeleteProgram(g_shaderProgram);

}

