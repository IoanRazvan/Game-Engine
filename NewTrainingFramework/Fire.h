#pragma once
#include "SceneObject.h"
class Fire :
	public SceneObject
{
private:
	float u_Time;
	float u_DispMax;
	void updateImpl() override;
	void sendUniqueData() override;
public:
	Fire(int id, Vector3& position, Vector3& rotation, Vector3& scale, Vector3& color, int model, int shader, const std::vector<int>& textures, bool wired, float dispMax);
	Fire(int id, Vector3& position, Vector3& rotation, Vector3& scale, Vector3& color, Model* model, Shader* shader, const std::vector<Texture*>& textures, bool wired, float dispMax);
};

