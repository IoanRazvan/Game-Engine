#include "stdafx.h"
#include "Model.h"
#include <iostream>

Model::Model(ModelResource *mr) : mr(mr)
{}

Model::~Model()
{
	glDeleteBuffers(3, bufferIds);
}

void Model::setBufferSize(Buffer type, int size)
{
	if (type == Buffer::FILLED_INDICES)
		numberOfFilledIndices = size;
	else if (type == Buffer::WIRED_INDICES)
		numberOfWiredIndices = size;
}

int Model::getBufferSize(Buffer type) const
{
	if (type == Buffer::FILLED_INDICES)
		return numberOfFilledIndices;
	else if (type == Buffer::WIRED_INDICES)
		return numberOfWiredIndices;
	return -1;
}

void Model::load()
{
	std::vector<GLushort> filledIndicesArray;
	std::vector<GLushort> wiredIndicesArray;

	LoadNFG(mr->path + mr->fileName, verticesArray, filledIndicesArray);
	wiredIndicesArray = getWiredModelIndices(filledIndicesArray);

	numberOfFilledIndices = filledIndicesArray.size();
	numberOfWiredIndices = wiredIndicesArray.size();

	glGenBuffers(3, bufferIds);

	glBindBuffer(GL_ARRAY_BUFFER, bufferIds[static_cast<int>(Buffer::VERTICES)]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verticesArray.size(), &verticesArray[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIds[static_cast<int>(Buffer::FILLED_INDICES)]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * filledIndicesArray.size(), &(filledIndicesArray[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIds[static_cast<int>(Buffer::WIRED_INDICES)]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * wiredIndicesArray.size(), &(wiredIndicesArray[0]), GL_STATIC_DRAW);
}