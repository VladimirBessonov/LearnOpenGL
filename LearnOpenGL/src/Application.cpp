#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
//
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX =0, FRAGMENT =1
    };
    ShaderType type = ShaderType::NONE;
    std::stringstream ss[2];
    std::string line;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;

        }
        else {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };

};


static int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {

        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "Vertex":"Fragement ") <<" Shader" <<std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;

    }
    return id;
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(840, 840, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK) std::cout << "Error with GLEW" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
    /* Loop until the user closes the window */
    float positions[] = {
    -0.5f, -0.5,
    0.5f,-0.5f,
    0.5f, 0.5f,
     -0.5f, 0.5f,
    };

    unsigned int indices[] = {

        0, 1, 2,
        2, 3, 0
    };

    // For Core Profile create VAO !!
    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));
    // unsigned int buffer;
    
     VertexBuffer vb(positions, sizeof(positions));

     GLCall(glEnableVertexAttribArray(0));
     GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    IndexBuffer ib(indices, 6);

    ShaderProgramSource shaderSource = ParseShader("res/shaders/Basic.shader");

    unsigned int shader = CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
    glUseProgram(shader);
    int location = glGetUniformLocation(shader, "u_color");
    ASSERT(location != -1);
    glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f);

    float b = 0.0f;
    float increment = 0.05f;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        // glBegin(GL_TRIANGLES);
         glEnableVertexAttribArray(0);
        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, 0.2f, 0.3f, b, 1.0f));
        GLCall(glBindVertexArray(vao));
        //vb.Bind();
         ib.Bind();
       /* glDrawArrays(GL_TRIANGLES, 0, 3);*/
        
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); //UNSIGNED_ simulate error 
      //  glDisableVertexAttribArray(0);
        /* Swap front and back buffers */
        if (b > 1.0f) 
            increment = -0.05f;
        else if (b < 0.0f)
            increment = 0.05f;
        b += increment;
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}
