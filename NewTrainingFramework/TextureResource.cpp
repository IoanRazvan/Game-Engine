#include "stdafx.h"
#include "TextureResource.h"

GLint TextureResource::getFilter(const std::string& filter) {
	if (filter == "LINEAR")
		return GL_LINEAR;
	else if (filter == "NEAREST")
		return GL_NEAREST;
	return -1;
}

GLint TextureResource::getWrap(const std::string& wrap) {
	if (wrap == "CLAMP_TO_EDGE")
		return GL_CLAMP_TO_EDGE;
	else if (wrap == "REPEAT")
		return GL_REPEAT;
	return -1;
}

GLenum TextureResource::getType(const std::string& type) {
	if (type == "2d")
		return GL_TEXTURE_2D;
	else if (type == "cube")
		return GL_TEXTURE_CUBE_MAP;
	return -1;
}

TextureResource::TextureResource(const std::string& path, const std::string& type,const std::string& fileName, const std::string& minFilter, const std::string& magFilter,
	const std::string& wrapS, const std::string& wrapT) : path(path), fileName(fileName)
{
	this->type = getType(type);
	this->minFilter = getFilter(minFilter);
	this->magFilter = getFilter(magFilter);
	this->wrapS = getWrap(wrapS);
	this->wrapT = getWrap(wrapT);
}
