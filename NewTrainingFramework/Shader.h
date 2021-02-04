#pragma once
#include "../Utilities/utilities.h"
#include "ShaderResource.h"
#include <map>
#include <string>
#include <memory>

class Shader {
private:
	ShaderResource* sr;
	std::map<std::string, int> uniforms;
	std::map<std::string, int> attributes;
	unsigned int program;
	unsigned int vertexShader;
	unsigned int fragmentShader;
public:
	typedef ShaderResource resourceType;
	Shader(ShaderResource* sr);
	~Shader();
	int getAttrib(const std::string& attribName);
	int getUniform(const std::string& uniformName);
	void load();
	void bind() const;
};
