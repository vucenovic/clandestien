#include "KeyInput.h"

std::vector<KeyInput*> KeyInput::instances;

KeyInput::KeyInput(std::vector<int> keysToMonitor) {
	for (int key : keysToMonitor) {
		keys[key] = false;
	}
	KeyInput::instances.push_back(this);
}

bool KeyInput::getKeyDown(int key) {
	bool result = false;
	std::map<int, bool>::iterator it = keys.find(key);
	if (it != keys.end()) {
		result = keys[key];
	}
	return result;
}

void KeyInput::setKeyDown(int key, bool isDown) {
	std::map<int, bool>::iterator it = keys.find(key);
	if (it != keys.end()) {
		keys[key] = isDown;
	}
}

void KeyInput::setupKeyInputs(GLFWwindow* window) {
	glfwSetKeyCallback(window, KeyInput::callback);
}

void KeyInput::callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	for (KeyInput* keyInput : instances) {
		keyInput->setKeyDown(key, action != GLFW_RELEASE);
	}
}