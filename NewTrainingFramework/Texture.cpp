#include "stdafx.h"
#include "Texture.h"
#include "../Utilities/utilities.h"
#include <iostream>

Texture::Texture(TextureResource * tr) : tr(tr)
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureId);
}

void Texture::load()
{
	std::string fullFileName = tr->path + tr->fileName;
	int width, height, bpp;
	std::unique_ptr<char> pixels = std::unique_ptr<char>(LoadTGA(&(fullFileName[0]), &width, &height, &bpp));
	
	glGenTextures(1, &textureId);

	glBindTexture(tr->type, textureId);

	glTexParameteri(tr->type, GL_TEXTURE_MIN_FILTER, tr->minFilter);
	glTexParameteri(tr->type, GL_TEXTURE_MAG_FILTER, tr->magFilter);
	glTexParameteri(tr->type, GL_TEXTURE_WRAP_S, tr->wrapS);
	glTexParameteri(tr->type, GL_TEXTURE_WRAP_T, tr->wrapT);

	if (tr->type == GL_TEXTURE_2D)
		glTexImage2D(tr->type, 0, bpp == 32 ? GL_RGBA : GL_RGB, width, height, 0, bpp == 32 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels.get());
	else if (tr->type == GL_TEXTURE_CUBE_MAP)
	{
		int bytesPerTexel = bpp / 8;
		int offSets[6] = { (width / 2 + (width * height / 3)), (height * width / 3), (width / 4),
						((2 * height * width / 3) + width / 4),  ((height * width / 3) + width / 4), ((height * width / 3) + (3 * width / 4)) };
		for (int i = 0; i < 6; i++) {
			loadCubeTextureFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, width / 4, width, height/3, bpp == 32 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, offSets[i] * bytesPerTexel, pixels.get());
		}
	}
}

void Texture::loadCubeTextureFace(GLenum target, int faceWidth, int bigWidth, int faceHeight, GLenum format, GLenum type, int ptrOffset, const char* ptr)
{
	int bytesPerTexel = format == GL_RGBA ? 4 : 3;
	glTexImage2D(target, 0, format, faceWidth, faceHeight, 0, format, type, 0);
	for (int i = 0; i < faceHeight; i++) {
		glTexSubImage2D(target, 0, 0, i, faceWidth, 1, format, type, ptr + ptrOffset + (bigWidth)*i * bytesPerTexel);
	}
}