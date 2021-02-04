#pragma once

#include "../Utilities/utilities.h"
#include "Vertex.h"
#include <vector>
#include <string>

void LoadNFG(const std::string& fileName, std::vector<Vertex>& vertexArray, std::vector<GLushort>& indexArray);
std::vector<GLushort> getWiredModelIndices(const std::vector<GLushort>& filledModelIndicesArray);