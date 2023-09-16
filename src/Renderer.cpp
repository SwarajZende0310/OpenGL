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

