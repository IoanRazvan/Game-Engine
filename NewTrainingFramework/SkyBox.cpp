#include "stdafx.h"
#include "SkyBox.h"
#include "SceneManager.h"

SkyBox::SkyBox(int id_, Vector3& position_, Vector3& rotation_, Vector3& scale_, Vector3& color_,
	Vector3& followingCamera_, Model* modelPtr, Shader* shaderPtr, const std::vector<Texture*> vTexturePtr, bool wired)
	: SceneObject(id_, position_, rotation_, scale_, color_, followingCamera_, modelPtr, shaderPtr, vTexturePtr, wired), offsetY(position_.y)
{
}

SkyBox::SkyBox(int id_, Vector3& position_, Vector3& rotation_, Vector3& scale_, Vector3& color_,
	Vector3& followingCamera_, int modelId, int shaderId, const std::vector<int> vTextureIds, bool wired)
	: SceneObject(id_, position_, rotation_, scale_, color_, followingCamera_, modelId, shaderId, vTextureIds, wired), offsetY(position_.y)
{
}

void SkyBox::sendUniqueData()
{
}

