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
        float positions[] = {
            100.f, 100.f, 0.0f, 0.0f, //0 
            200.f, 100.f, 1.0f, 0.0f, //1
            200.f, 200.f, 1.0f, 1.0f, //2
            100.f, 200.f, 0.0f, 1.0f  //3
        };

        unsigned int indices[] = {
            0,1,2,
            2,3,0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));
        
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb,layout);


        IndexBuffer ib(indices, 6);

        //Fixing the projection of our window ny default it renders for 1x1 matrix but ours is 4x3
        //Hence use projection matrix
        //glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);//As 4x3 window left = -2 : right = 2 : bottom = -1.5 : up = 1.5
        glm::mat4 proj = glm::ortho(0.f, 960.f, 0.f, 540.f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(-100.f, 0.f, 0.f));//For camera :: moving camera to right THUS moving object to left
        glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(200.f, 200.f, 0.f));//For Model
        glm::mat4 mvp = proj * view * model; //(Multiplied in reverse order becuase in OpenGL it is Column Major)

        std::string filepath = "res/shaders/BasicShader.txt";
        Shader shader(filepath);
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.f);//Setting the value of fragment shader from CPU
        shader.SetUniformMat4f("u_MVP", mvp);

        std::string texPath = "res/textures/ChernoLogo.png";
        Texture texture(texPath);
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        //Unbind everything
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;
        float r = 0.0f;
        float increement = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
            
            //Draw using renderer
            renderer.Draw(va,ib,shader);

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