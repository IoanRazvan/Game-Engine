#pragma once
#include <string>

struct ModelResource {
	std::string path;
	std::string fileName;
	ModelResource(const std::string& path, const std::string& fileName);
	ModelResource() = default;
};

