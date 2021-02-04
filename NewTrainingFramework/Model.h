#pragma once
#include "NFG.h"
#include "ModelResource.h"
#include <memory>

class Model
{
public:
	enum class Buffer { VERTICES = 0, FILLED_INDICES = 1, WIRED_INDICES = 2 };
private:
	int numberOfFilledIndices, numberOfWiredIndices;
	GLuint bufferIds[3];
	ModelResource* mr;
	std::vector<Vertex> verticesArray;
public:
	typedef ModelResource resourceType;
	Model(ModelResource* mr);
	Model(std::vector<Vertex> verticesArray) : mr(nullptr), verticesArray(verticesArray) {}
	~Model();
	void load();
	int getBufferSize(Buffer type) const;
	GLuint getBufferId(Buffer type) const { return bufferIds[static_cast<int>(type)]; }

	// used by Terrain objects to set the properties once they
	// generate their model
	void setBufferSize(Buffer type, int size);
	void setBufferId(Buffer type, int id) { bufferIds[static_cast<int>(type)] = id; }
};

