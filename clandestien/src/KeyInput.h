#pragma once

#include <vector>
#include <map>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>

class KeyInput {
public:
	KeyInput(std::vector<int> keysToMonitor);
	bool getKeyDown(int key);
	static void setupKeyInputs(GLFWwindow* window);

private:
	void setKeyDown(int key, bool isDown);
	std::map<int, bool> keys;
	static void callback(
		GLFWwindow* window, int key, int scancode, int action, int mods);
	static std::vector<KeyInput*> instances;
};

