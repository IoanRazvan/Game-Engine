#include "stdafx.h"
#include "Fire.h"
#include <ctime>
#include <cmath>


Fire::Fire(int id, Vector3& position, Vector3& rotation, Vector3& scale, Vector3& color, int model, int shader, const std::vector<int>& textures, bool wired, float dispMax)
	: SceneObject(id, position, rotation, scale, color, Vector3(1, 0, 0), model, shader, textures, wired), u_DispMax(dispMax), u_Time(0.0f)
{
}

Fire::Fire(int id, Vector3& position, Vector3& rotation, Vector3& scale, Vector3& color, Model* model, Shader* shader, const std::vector<Texture*>& textures, bool wired, float dispMax)
	: SceneObject(id, position, rotation, scale, color, Vector3(1, 0, 0), model, shader, textures, wired), u_DispMax(dispMax), u_Time(0.0f)
{
}

void Fire::updateImpl()
{
	float currentTime = static_cast<float>(clock()) / CLOCKS_PER_SEC;
	u_Time = currentTime;
}

void Fire::sendUniqueData()
{
	Shader* ptrToShader = SceneObject::getPtrToShader();
	if (ptrToShader->getUniform("u_Time") != -1)
		glUniform1f(ptrToShader->getUniform("u_Time"), u_Time);
	if (ptrToShader->getUniform("u_DispMax") != -1)
		glUniform1f(ptrToShader->getUniform("u_DispMax"), u_DispMax);
}