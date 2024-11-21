#ifndef TEXT_H
#define TEXT_H

#include <iostream>
#include <glm.hpp>
#include "Shader.h"
#include <map>

using namespace std;

struct Character {
	unsigned int TextureID; // ID
	glm::ivec2	 Size; // Tamaño 
	glm::ivec2	 Bearing; // Offset desde l alínea base hasta el top o la izq
	unsigned int Advance; // Offset hacia el siguiente glifo
};

class Text {
	public:
		//Shader textShader;
		unsigned int VAO, VBO;
		map<char, Character> Characters;
		void CleanGL();
		//virtual void CleanGL();
		Text(unsigned int height, unsigned int width);
		void LoadText(string font, unsigned int size);
		void RenderText(string text, float x, float y, float scale, glm::vec3 color, Shader s);
};

#endif