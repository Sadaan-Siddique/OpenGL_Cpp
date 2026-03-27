// When we’re talking about camera/view space we’re talking about all the vertex coordinates as seen from the camera’s perspective as the origin of the scene: the view matrix transforms all the world coordinates into view coordinates that are relative to the camera’s position and direction.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

// Shader
const char* g_vertexShaderSource = R"( 
    #version 330 core
    layout (location = 0) in vec3 aPos; 
    // layout (location = 1) in vec3 aColor;
    layout (location = 1) in vec2 aTexCoord; 

    // out vec3 ourColor;
    out vec2 TexCoord;

    uniform mat4 model; // <--- The new transformation matrix
    uniform mat4 view; 
    uniform mat4 projection;


    void main()
    {
        // Matrix multiplication goes strictly from right to left!
        // gl_Position = transform * vec4(aPos, 1.0);
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        
        // ourColor = aColor;
        TexCoord = aTexCoord;
    }
)";

const char* g_fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"

    // "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"

    "uniform sampler2D texture1;\n" // The first image
    "uniform sampler2D texture2;\n" // The second image
    "uniform float mixValue;\n"

    "void main()\n"
    "{\n"
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
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };

    // unsigned int indices[] = {
    //     0, 1, 3,
    //     1, 2, 3
    // };

    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);

    glGenBuffers(1, &g_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glGenBuffers(1, &g_EBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color Attribute
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void*)(3* sizeof(float)));
    // glEnableVertexAttribArray(1); 

    // Texture Attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
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
    // To safely reuse the variables for two images, you must completely finish setting up the first texture and call stbi_image_free(data) before you try to load the second one.    
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
    
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        g_mixValue += 0.005f; // Change this number to make it fade faster or slower
        if(g_mixValue >= 1.0f)
            g_mixValue = 1.0f; // will Lock it at 1.0 maximum
    }

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

        glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f, 0.0f, 0.0f),
            glm::vec3( 2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f, 2.0f, -2.5f),
            glm::vec3( 1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)
        };

    while(!glfwWindowShouldClose(g_window))
    {
        processInput(g_window);
        
        // Pre Draw
        // glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glClearColor(1.f, 1.f, 0.f, 1.f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Without this, Framebuffer may behave weirdly.

        glUseProgram(g_shaderProgram); // to actviate the shader

        // This example creates a view matrix that is the same as the one we created in the cube .cpp file
        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        // glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraPos = glm::vec3(camX, 0.0f, camZ);
        
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

        glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

        glm::mat4 view_matrix;
        view_matrix = glm::lookAt(cameraPos, cameraTarget, up); // The glm::LookAt function requires a position, target and up vector respectively.
        
        
        // PROJECTION: Create the 3D perspective (45-degree FOV, 800x800 aspect ratio)
        // Here is exactly how your code fails without the Projection Matrix:
        // Your cube starts at Z = 0.0.
        // Your View Matrix pushes the cube away from the camera to Z = -3.0.
        // Because you deleted the Projection Matrix, the vertex leaves the shader with a Z-coordinate of -3.0.
        // OpenGL looks at -3.0, sees that it is way outside the allowed -1.0 to 1.0 limit, and instantly deletes every single triangle before they can be drawn to your screen.
        glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);

        // Now, sending these matrices to the vertex shader
        // unsigned int viewLoc = glGetUniformLocation(g_shaderProgram, "view");
        unsigned int viewLoc = glGetUniformLocation(g_shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(g_shaderProgram, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        // for texture 1
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_texture1);
        glUniform1i(glGetUniformLocation(g_shaderProgram, "texture1"), 0);

        // for texture 2
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_texture2);
        glUniform1i(glGetUniformLocation(g_shaderProgram, "texture2"), 1);

        glUniform1f(glGetUniformLocation(g_shaderProgram, "mixValue"), g_mixValue); // Send the real-time C++ float to the GLSL uniform
        
        // Draw
        glBindVertexArray(g_VAO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        unsigned int modelLoc = glGetUniformLocation(g_shaderProgram, "model");
        for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model_matrix = glm::mat4(1.0f);
            model_matrix = glm::translate(model_matrix, cubePositions[i]);
            float angle = 20.0f * i;
            if (i % 3 == 0) angle += (float)glfwGetTime() * 50.0f;
            model_matrix = glm::rotate(model_matrix,( glm::radians(angle)), glm::vec3(1.0f, 0.3f, 0.5f));   
            // ourShader.setMat4("model", model);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    GetOpenGLVersionInfo();

    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
    glDeleteProgram(g_shaderProgram);

}
