#include"TestTexture2D.h"
#include"Renderer.h"
#include"imgui/imgui.h"

#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"

namespace test {
	TestTexture2D::TestTexture2D(): 
        m_Proj(glm::ortho(0.f, 960.f, 0.f, 540.f, -1.0f, 1.0f)),
        m_View(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f))),
        m_translationA(200.f,200.f,0.f), m_translationB(400.f, 200.f, 0.f)
	{

        float positions[] = {
            -50.f, -50.f, 0.0f, 0.0f, //0 
             50.f, -50.f, 1.0f, 0.0f, //1
             50.f,  50.f, 1.0f, 1.0f, //2
            -50.f,  50.f, 0.0f, 1.0f  //3
        };

        unsigned int indices[] = {
            0,1,2,
            2,3,0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        m_VBO=std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        m_VAO->AddBuffer(*m_VBO, layout);


        m_IBO = std::make_unique<IndexBuffer>(indices, 6);

        //Fixing the projection of our window ny default it renders for 1x1 matrix but ours is 4x3
        //Hence use projection matrix
        //glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);//As 4x3 window left = -2 : right = 2 : bottom = -1.5 : up = 1.5
        //m_Proj = ;
        //m_View = ;//For camera :: moving camera to right THUS moving object to left
        //glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(200.f, 200.f, 0.f));//For Model
        //glm::mat4 mvp = proj * view * model; //(Multiplied in reverse order becuase in OpenGL it is Column Major)
        glm::vec3 translationA(200.f, 200.f, 0.f);
        glm::vec3 translationB(400.f, 200.f, 0.f);

        std::string filepath = "res/shaders/BasicShader.txt";
        m_Shader = std::make_unique<Shader>(filepath);
        m_Shader->Bind();

        std::string path = "res/textures/ChernoLogo.png";
        m_Texture = std::make_unique<Texture>(path);
        m_Shader->SetUniform1i("u_Texture", 0);

	}

	TestTexture2D::~TestTexture2D()
	{
	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{
	}

	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.f, 0.f, 0.f, 1.f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;
        m_Texture->Bind();
        {
            glm::mat4 model = glm::translate(glm::mat4(1.f), m_translationA);//For Model
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            //Draw using renderer
            renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.f), m_translationB);//For Model
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            //Draw using renderer
            renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
        }
	}

	void TestTexture2D::OnImGuiRender()
	{
        ImGui::SliderFloat3("translationA", &m_translationA.x, 0.0f, 960.0f);
        ImGui::SliderFloat3("translationB", &m_translationB.x, 0.0f, 960.0f);
        
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}
