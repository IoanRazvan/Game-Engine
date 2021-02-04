#include "stdafx.h"
#include "Shader.h"
#include <iostream>

Shader::Shader(ShaderResource* sr) : sr(sr)
{}

Shader::~Shader()
{
	glDeleteProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::load()
{
	std::string fullVertexShaderName = sr->path + sr->vertexShader;
	std::string fullFragmentShaderName = sr->path + sr->fragmentShader;

	vertexShader = esLoadShader(GL_VERTEX_SHADER, &(fullVertexShaderName[0]));

	if (vertexShader == 0) {
		std::cout << "Problem while loading vertex shader!";
		return;
	}

	fragmentShader = esLoadShader(GL_FRAGMENT_SHADER, &(fullFragmentShaderName[0]));

	if (fragmentShader == 0) {
		std::cout << "Problem while loading fragment shader!";
		glDeleteShader(vertexShader);
		return;
	}

	program = esLoadProgram(vertexShader, fragmentShader);
}

void Shader::bind() const
{
	glUseProgram(program);
}

int Shader::getAttrib(const std::string& attribName) {
	if (attributes.find(attribName) == attributes.end())
		attributes[attribName] = glGetAttribLocation(program, attribName.c_str());
	return attributes[attribName];
}

int Shader::getUniform(const std::string& uniformName) {
	if (uniforms.find(uniformName) == uniforms.end())
		uniforms[uniformName] = glGetUniformLocation(program, uniformName.c_str());
	return uniforms[uniformName];
}