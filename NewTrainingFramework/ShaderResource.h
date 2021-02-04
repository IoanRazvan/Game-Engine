#pragma once
#include <string>

struct ShaderResource {
	std::string path;
	std::string vertexShader;
	std::string fragmentShader;
	ShaderResource(const std::string& path, const std::string& vertexShader,
		const std::string& fragmentShader);
	ShaderResource() = default;
};

