#pragma once
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>

void MyKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);