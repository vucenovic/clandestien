#pragma once

#include <GL\glew.h>

#include <vector>
#include <fstream>
#include <iostream> 
#include "Shader.h"



class HUD
{
	private:

		GLuint texture2D;

	public:
		
		void addHUD(const char* filepath);
		void drawHUDquad(int width, int height);

};




