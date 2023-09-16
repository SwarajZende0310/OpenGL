#pragma once

#include<GL/glew.h>


#define ASSERT(x) if (!(x))__debugbreak(); //Break the debugging if x returns false
#define GLCall(x) GLClearError();\
x;\
ASSERT(GLLogCall(#x,__FILE__,__LINE__));//Wrap a funciton with error boundary

//Clears all unrelated errors
void GLClearError();

/*
Check for an error and log the error to the console.

@param function The function where the error happend
@param file The file where the error happend
@param line The line number where the error happend
@return Whether there was an error
*/
bool GLLogCall(const char* function, const char* file, int line);