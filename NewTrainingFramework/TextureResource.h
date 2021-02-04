#pragma once
#include <string>
#include "../Utilities/utilities.h"

struct TextureResource {
	std::string path;
	std::string fileName;
	GLenum type;
	GLint minFilter;
	GLint magFilter;
	GLint wrapS;
	GLint wrapT;

	TextureResource(const std::string& path, const std::string& type, const std::string& fiileName,
		const std::string& minFilter, const std::string& magFilter,
		const std::string& wrapS, const std::string& wrapT);
	TextureResource() = default;
	static GLint getFilter(const std::string& filter);
	static GLint getWrap(const std::string& wrap);
	static GLenum getType(const std::string& type);
};

