	#include "stdafx.h"
#include "SceneObject.h"
#include "ResourceManager.h"
#include "SceneManager.h"


void SceneObject::buildObjectModelMatrix()
{
	Vector3 cameraPosition = SceneManager::getInstance()->getActiveCamera()->getPosition();
	Vector3 positionWithFollowing = position;
	if (followingCamera.x == 1.0f) {
		positionWithFollowing.x += cameraPosition.x;
	}

	if (followingCamera.y == 1.0f) {
		positionWithFollowing.y += cameraPosition.y;
	}

	if (followingCamera.z == 1.0f) {
		positionWithFollowing.z += cameraPosition.z;
	}

	objectModel = Matrix().SetScale(scale) * (Matrix().SetRotationX(rotation.x)
		* Matrix().SetRotationY(rotation.y) * Matrix().SetRotationZ(rotation.z))
		* Matrix().SetTranslation(positionWithFollowing);
}

SceneObject::SceneObject(int id_, Vector3& position_, Vector3& rotation_, Vector3& scale_, Vector3& color_,
	Vector3& followingCamera_, Model* modelPtr, Shader* shaderPtr, const std::vector<Texture*> vTexturePtr, bool wired)
	: objectId(id_), position(position_), rotation(rotation_), scale(scale_), ptrToModel(modelPtr),
	ptrToShader(shaderPtr), vPtrToTexture(vTexturePtr), color(color_), wired(wired), followingCamera(followingCamera_)
{
	buildObjectModelMatrix();
}

SceneObject::SceneObject(int id_, Vector3& position_, Vector3& rotation_, Vector3& scale_, Vector3& color_,
	Vector3& followingCamera_, int modelId, int shaderId, const std::vector<int> vTextureIds, bool wired)
	: objectId(id_), position(position_), rotation(rotation_), scale(scale_), color(color_),
	wired(wired), followingCamera(followingCamera_)
{
	ResourceManager* rm = ResourceManager::getInstance();
	if (modelId != rm->GENERATED_MODEL_ID)
		ptrToModel = rm->Load<Model>(modelId);
	else
		ptrToModel = nullptr;
	ptrToShader = rm->Load<Shader>(shaderId);
	for (auto texture : vTextureIds)
		vPtrToTexture.push_back(rm->Load<Texture>(texture));

	buildObjectModelMatrix();
}

void SceneObject::draw()
{
	ptrToShader->bind();

	glBindBuffer(GL_ARRAY_BUFFER, ptrToModel->getBufferId(Model::Buffer::VERTICES));

	sendCommonData();
	sendUniqueData();

	if (wired == false && SceneManager::getInstance()->getDebugModeStatus() == false) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptrToModel->getBufferId(Model::Buffer::FILLED_INDICES));
		glDrawElements(GL_TRIANGLES, ptrToModel->getBufferSize(Model::Buffer::FILLED_INDICES), GL_UNSIGNED_SHORT, 0);
	}
	else {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptrToModel->getBufferId(Model::Buffer::WIRED_INDICES));
		glDrawElements(GL_LINES, ptrToModel->getBufferSize(Model::Buffer::WIRED_INDICES), GL_UNSIGNED_SHORT, 0);
	}

	if (SceneManager::getInstance()->getDebugModeStatus() == true) {
		SceneManager::getInstance()->drawDebugAxis(Matrix().SetRotationX(rotation.x) * Matrix().SetRotationY(rotation.y)* Matrix().SetRotationZ(rotation.z) * Matrix().SetTranslation(position));
	}
}

void SceneObject::sendCommonData()
{

	if (ptrToShader->getAttrib("a_posL") != -1) {
		glEnableVertexAttribArray(ptrToShader->getAttrib("a_posL"));
		glVertexAttribPointer(ptrToShader->getAttrib("a_posL"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	}

	if (ptrToShader->getUniform("u_matrix") != -1)
		glUniformMatrix4fv(ptrToShader->getUniform("u_matrix"), 1, GL_FALSE, &(objectModel * (SceneManager::getInstance()->getActiveCamera())->getMVP()).m[0][0]);

	if (ptrToShader->getAttrib("a_color") != -1) {
		glDisableVertexAttribArray(ptrToShader->getAttrib("a_color"));
		glVertexAttrib3f(ptrToShader->getAttrib("a_color"), color.x, color.y, color.z);
	}

	int i;
	for (i = 0; i < vPtrToTexture.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(vPtrToTexture[i]->getTextureType(), vPtrToTexture[i]->getTextureId());
		std::string textureName = "u_texture_" + std::to_string(i);
		glUniform1i(ptrToShader->getUniform(textureName), i);
	}

	if (ptrToShader->getUniform("u_fog_texture") != -1) {
		SceneManager* sm = SceneManager::getInstance();

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, sm->getFogTextureId());
		glUniform1i(ptrToShader->getUniform("u_fog_texture"), i);

		int err = glGetError();

		if (ptrToShader->getUniform("r") != -1) {
			float r = sm->getr();
			glUniform1f(ptrToShader->getUniform("r"), r);
		}

		if (ptrToShader->getUniform("R") != -1) {
			float R = sm->getR();
			glUniform1f(ptrToShader->getUniform("R"), R);
		}
	}

	if (ptrToShader->getAttrib("a_uv") != -1) {
		glEnableVertexAttribArray(ptrToShader->getAttrib("a_uv"));
		glVertexAttribPointer(ptrToShader->getAttrib("a_uv"), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(5 * sizeof(Vector3)));
	}

	if (ptrToShader->getUniform("u_cameraPos") != -1) {
		Vector3 cameraPosition = SceneManager::getInstance()->getActiveCamera()->getPosition();
		glUniform3f(ptrToShader->getUniform("u_cameraPos"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
	}

	if (ptrToShader->getUniform("u_model") != -1) {
		glUniformMatrix4fv(ptrToShader->getUniform("u_model"), 1, GL_FALSE, reinterpret_cast<const float*>(objectModel.m));
	}
}

void SceneObject::sendUniqueData()
{
	if (ptrToShader->getAttrib("a_norm") != -1) {
		glEnableVertexAttribArray(ptrToShader->getAttrib("a_norm"));
		glVertexAttribPointer(ptrToShader->getAttrib("a_norm"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(2 * sizeof(Vector3)));
	}
}

void SceneObject::updateImpl()
{
	buildObjectModelMatrix();
}

void SceneObject::update()
{
	updateImpl();
}