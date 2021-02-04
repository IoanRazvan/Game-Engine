#pragma once
#include "SceneObject.h"

class SkyBox : public SceneObject
{
private:
	float offsetY;
	void sendUniqueData();
public:
	SkyBox(int id_, Vector3& position_, Vector3& rotation_, Vector3& scale_, Vector3& color_,
		Vector3& followingCamera, Model* modelPtr, Shader* shaderPtr, const std::vector<Texture*> vTexturePtr, bool wired);
	SkyBox(int id_, Vector3& position_, Vector3& rotation_, Vector3& scale_, Vector3& color_,
		Vector3& followingCamera_, int modelId, int shaderId, const std::vector<int> vTextureIds, bool wired);
};

