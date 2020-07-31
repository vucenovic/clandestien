#include "KeyRiddle.h"

KeyRiddle::KeyRiddle(std::string code) : code(code)
{
}

void KeyRiddle::keyInput(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		int debug = key;
		auto debugVal = code[status];
		int statusDebug = status;
		if (code[status] == key) {
			status++;
		}
		else {
			status = 0;
		}
		if (status == code.length()) {
			riddleSolved = true;
		}
	}
		
}

