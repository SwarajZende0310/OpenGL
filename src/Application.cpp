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
#include"Shader.h"

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

        std::string filepath = "res/shaders/BasicShader.txt";
        Shader shader(filepath);
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.f);//Setting the value of fragment shader from CPU

        //Unbind everything
        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();


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
            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.f);

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