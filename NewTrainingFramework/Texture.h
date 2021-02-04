#pragma once
#include "TextureResource.h"
#include <memory>

class Texture
{
private:
	TextureResource* tr;
	GLuint textureId;
	void loadCubeTextureFace(GLenum target, int width, int bigWidth, int height, GLenum format, GLenum type, int ptrOffset, const char *);
public:
	typedef TextureResource resourceType;
	Texture(TextureResource* tr);
	~Texture();
	GLuint getTextureId() const { return textureId; }
	GLenum getTextureType() const { return tr->type; }
	void load();
};

