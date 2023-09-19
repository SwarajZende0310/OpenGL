#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

#include"Renderer.h"

#include"VertexBuffer.h"
#include"VertexBufferLayout.h"
#include"IndexBuffer.h"
#include"VertexArray.h"
#include"Shader.h"
#include"Texture.h"

#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"

#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw_gl3.h"

#include"tests/TestClearColor.h"

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
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
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
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        test::TestClearColor test;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            test.OnUpdate(0.f);
            test.OnRender();

            ImGui_ImplGlfwGL3_NewFrame();

            test.OnImGuiRender();

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLCall(glfwPollEvents());
        }
    }

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
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

  --TEXTURES::
  2D OR 3D Images applied to the surfaces of a 3D Model
  Image used while we are rendering something

  --BLENDING::
  Determines how we combine our OUTPUT color with what is already in our target buffer
  ::OUTPUT = the color we output from our fragemnt shader (known as SOURCE)
  ::TARGET BUFFER = buffer our fragment shader is drawing to (known as DESTINATION)

  :::Control Blending-
  == 3 ways :--
  1.glEnable(GL_BLEND) - glDisable(GL_BLEND)
  2.glBlendFunc(src,dest)
     src = how the src RGBA factor is computed (default is GL_ONE)
     dest = how the dest RGBA facotr is computed (default is GL_ZERO)
  3.glBlendEquation(mode)
     mode = how we combine the src and dest
     Default value is GL_FUNC_ADD (i.e. just adds src and dest)

  --Maths::
  1.Matrices(for Transformations)
  2.Vectors

  Projections ::
  Mapping coordinates in 3D or 2D World/Space to our actual screen(Converts to Normalised Device Coordinates)
  (Maths that converts 3D Points in world to 2D Points in the Window of our Screen)
  ::Normalised Coordinates == Left = -1, Right = 1, Top = 1, Bottom = -1; 
  ___ 2 Types ___
  1.Orthographic projection(Usually 2D i.e.farther the object does not get smaller)
  2.Perspective projection(Usually 3D i.e.farther the object smaller it is)

  __MODEL VIEW PROJECTION(MVP)__
  All the 3 terms are individual matrices which are multiplied together (Multiplied in reverse order becuase in OpenGL it is Column Major)
  This RESULTANT is then multiplied by VERTEX_POSITIONS
  ::VIEW matrix == represents view of the camera(i.Position,Orientaion of the Camera)
     Moving the camera to the left is moving all the objects to right
  ::MODEL matrix == represents the actual objects we are rendering/drawing(i.e.Translation(Position),Rotaion,Scale of our Model)

*/