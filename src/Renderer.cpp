#include"Renderer.h"

#include<iostream>

void GLClearError() {
    //Clear all the error from other functions
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
    //Check for error from this function
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL ERROR] (" << error << "): " << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::Clear()const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    //Binding the object before every draw call 
    shader.Bind();
    va.Bind();
    ib.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));

    //Unbinding can be done but not strictly necessary as it increases overhead
    //Hence NOT DONE ;)
}
