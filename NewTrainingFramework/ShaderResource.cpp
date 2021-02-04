#include "stdafx.h"
#include "ShaderResource.h"

ShaderResource::ShaderResource(const std::string& path, const std::string& vertexShader, const std::string& fragmentShader)
	: path(path), vertexShader(vertexShader), fragmentShader(fragmentShader)
{
}