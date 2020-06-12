#pragma once

#include <vector>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include <map>
#include <functional>


class InputManager
{
private:
	static std::map<int, std::function<void(GLFWwindow*, int, int, int, int)>> callbacks;

public:
	InputManager() {};

	static void triggerCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods) {
		for (auto& [id, callback] : callbacks) {
			callback(window, key, scancode, action, mods);
		}
	}
	void setglfwcallback(GLFWwindow* window) {
		glfwSetKeyCallback(window, triggerCallbacks);
	}
	void registerCallback(int id, std::function<void(GLFWwindow*, int, int, int, int)> callback) { callbacks.insert(std::pair<int, std::function<void(GLFWwindow*, int, int, int, int)>>(id, callback)); };
	void unregisterCallback(int id) {
		auto element = callbacks.find(id);
		callbacks.erase(element);
	}

};

