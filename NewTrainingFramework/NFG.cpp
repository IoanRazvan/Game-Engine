#include "stdafx.h"
#include "NFG.h"
#include <iostream>
#include <fstream>
#include <exception>
#include <type_traits>

// template that reads the components of a vector
template <class T>
inline void readVectorComponents(std::ifstream& fin, T& aVector);
void readVertexArray(std::ifstream&, std::vector<Vertex>&);
void readIndexArray(std::ifstream&, std::vector<GLushort>&);

void LoadNFG(const std::string& filename, std::vector<Vertex>& vertexArray, std::vector<GLushort>& indexArray)
{
	std::ifstream fin(filename);

	if (fin.is_open() == false) {
		std::string errorMessage = "Unable to load model! File " + filename + " could not be loaded!";
		throw std::exception(errorMessage.c_str());
	}

	readVertexArray(fin, vertexArray);
	readIndexArray(fin, indexArray);
}

void readVertexArray(std::ifstream& fin, std::vector<Vertex>& vertexArray)
{
	while (!isdigit(fin.peek()))
		fin.get();
	int numberOfVertices;
	fin >> numberOfVertices;
	vertexArray.resize(numberOfVertices);

	Vector3 pos, norm, binorm, tgt;
	Vector2 uv;

	for (int i = 0; i < numberOfVertices; i++) {
		readVectorComponents(fin, pos);
		readVectorComponents(fin, norm);
		readVectorComponents(fin, binorm);
		readVectorComponents(fin, tgt);
		readVectorComponents(fin, uv);

		vertexArray[i].pos = pos;
		vertexArray[i].norm = norm;
		vertexArray[i].binorm = binorm;
		vertexArray[i].tgt = tgt;
		vertexArray[i].uv = uv;
	}
}

// readIndexArray should only be used after readVertexArray
void readIndexArray(std::ifstream& fin, std::vector<GLushort>& indexArray)
{
	while (!isdigit(fin.peek()))
		fin.get();
	int numberOfIndices;
	fin >> numberOfIndices;
	indexArray.resize(numberOfIndices);

	for (int i = 0; i < numberOfIndices; i += 3) {
		while (fin.get() != '.')
			;
		fin >> indexArray[i];
		fin.get();
		fin >> indexArray[i + 1];
		fin.get();
		fin >> indexArray[i + 2];
	}
}

template <class T>
void readVectorComponents(std::ifstream& fin, T& aVector)
{
	while (fin.get() != '[')
		;
	fin >> aVector.x;
	fin.get();
	fin >> aVector.y;
	if constexpr (std::is_same<T, Vector3>::value == true)
	{
		fin.get();
		fin >> aVector.z;
	}
}

std::vector<GLushort> getWiredModelIndices(const std::vector<GLushort>& filledModelIndicesArray)
{
	int filledSize = filledModelIndicesArray.size();
	if (filledSize == 0 || filledSize % 3 != 0)
		throw std::exception("The vector containing the indices of the filled model is invalid!");
	std::vector<GLushort> wired(3 * filledSize);
	for (int i = 0; i < filledSize; i += 3) {
		wired[2 * i] = filledModelIndicesArray[i];
		wired[2 * i + 1] = filledModelIndicesArray[i + 1];
		wired[2 * i + 2] = filledModelIndicesArray[i + 1];
		wired[2 * i + 3] = filledModelIndicesArray[i + 2];
		wired[2 * i + 4] = filledModelIndicesArray[i + 2];
		wired[2 * i + 5] = filledModelIndicesArray[i];
	}

	return wired;
}