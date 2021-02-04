#pragma once
#include "../Utilities/utilities.h"

class Shaders 
{
public:
	GLuint program, vertexShader, fragmentShader;
	char fileVS[260];
	char fileFS[260];
	GLint positionAttribute;
	GLint colorAttribute;
	GLint matrix_uniform;
	GLint texture_uniform;
	GLint uvAttribute;

	int Init(char * fileVertexShader, char * fileFragmentShader);
	~Shaders();
};