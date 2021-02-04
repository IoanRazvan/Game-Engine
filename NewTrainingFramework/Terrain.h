#pragma once
#include "SceneObject.h"
class Terrain :
	public SceneObject
{
private:
	int numberOfCells;
	int cellDimension;
	int offsetY;
	Vector3 heights;
	Vector2 offsets;
	void generateTerrain();
	void sendUniqueData();
	void updateImpl();
public:
	Terrain(int numberOfCells, int cellsDimension, int offsetY, Vector3& heights, int id, Vector3& position,
		Vector3& rotation, Vector3& scale, Vector3& color, Shader* shaderPtr, const std::vector<Texture*> vTexturePtr, bool wired);
	Terrain(int numberOfCells, int cellsDimension, int offsetY, Vector3& heights, int id_, Vector3& position_, Vector3& rotation_, Vector3& scale_, Vector3& color_,
		 int shaderId, const std::vector<int> vTextureIds, bool wired);
};

