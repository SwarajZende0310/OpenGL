#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

#include"Renderer.h"

#include"VertexBuffer.h"
#include"IndexBuffer.h"
#include"VertexArray.h"

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class Shadertype
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2]; // 2 for vertex and fragment shaders
    Shadertype type = Shadertype::NONE;

    while (getline(stream, line))
    {
        // Set the type if #shader is found
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = Shadertype::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = Shadertype::FRAGMENT;
        }
        else // Add the line to the source
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)(alloca(length * sizeof(char)));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile shader " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);//Delete intermidiate files
    glDeleteShader(fs);//Delete intermidiate files

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//Using version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_COMPAT_PROFILE);//Default mode genrates vetex array by default for us
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "ERROR !!!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;
    {
        float positions[] = {
            -0.5f, -0.5f, //0 
             0.5f, -0.5f, //1
             0.5f,  0.5f, //2
            -0.5f,  0.5f  //3
        };

        unsigned int indices[] = {
            0,1,2,
            2,3,0
        };

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexArray va;
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));
        
        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb,layout);


        IndexBuffer ib(indices, 6);

        ShaderProgramSource source = ParseShader("res/shaders/BasicShader.txt");
        std::cout << "VERTEX" << std::endl;
        std::cout << source.VertexSource << std::endl;
        std::cout << "FRAGMENT" << std::endl;
        std::cout << source.FragmentSource << std::endl;
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader));

        int location = glGetUniformLocation(shader, "u_Color");
        ASSERT(location != -1);
        GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.f));//Setting the value of fragment shader from CPU

        GLCall(glBindVertexArray(0));
        //Unbind everything
        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


        float r = 0.0f;
        float increement = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            /*
            //Legacy gl for checking whether OpenGL is working
            glBegin(GL_TRIANGLES);
            glVertex2f(-0.5f, -0.5f);
            glVertex2f( 0.f ,  0.5f);
            glVertex2f( 0.5f, -0.5f);
            glEnd();
            */

            //Binding the object before every draw call 
            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.f));

            va.Bind();
            ib.Bind();

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));


            if (r > 1.0f)increement = -0.05f;
            else if (r < 0.f)increement = 0.05f;

            r += increement;
            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLCall(glfwPollEvents());
        }

        glDeleteProgram(shader);
    }
    glfwTerminate();
    return 0;
}


/*
 ____________NOTES_______________

Buffer is data stored in GPU

Vetex is not a point it contains attributes more than a point like color , texture ,etc
SHADERS program running on GPU
  -Vertex shader --deals with the position of the vertex
  -Fragment shader --deals with colour every pixel also called PIXEL Shader
  calls(vertex shader) << calls(fragment shaders)


  --INDEX Buffer 
    for rendering a square which is made of 2 triangles but 
    this needs same points twice because GPU draws only using triangles
    therefore use index buffers to not repeat yourself rather use its indexes to draw it
   
  --Debugging OpenGL Errors
  ::GL Get Error==
  Throws error by error one at time(returns arbitary error flag)(so used in a loop to get all the errors)
  then call the function
  again call GLGetError in loop

  ::GL Debug Message Callback==
  Basically have to provide a callback function which will be called 
  when there is error in OpenGL
  (Better and easier to debug but availble with version above version 4.3)

  --UNIFORMS::
  used to get data from CPU to shaders(i.e.GPU)
  this are per draw i.e. if  2 triangles are being made in single draw we cannot noth with different colors
*/