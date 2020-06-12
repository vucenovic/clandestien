#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include <string>

class KeyRiddle
{
private:
	bool riddleSolved = false;
	std::string code;
	size_t status = 0;

public:
	KeyRiddle(std::string code);
	void keyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
	bool getSolved() { return riddleSolved; };
};

