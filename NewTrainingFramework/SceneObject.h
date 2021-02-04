#pragma once
#include "../Utilities/utilities.h"
#include "Model.h"
#include "Shader.h"	
#include "Texture.h"
#include "Camera.h"
#include <vector>

class SceneObject
{
private:
	int objectId;
	bool wired;
	Vector3 position, rotation, scale, color;
	Model* ptrToModel;
	Shader* ptrToShader;
	std::vector<Texture*> vPtrToTexture;
	Matrix objectModel;
	Vector3 followingCamera;

	void buildObjectModelMatrix();
	void sendCommonData();
	virtual void sendUniqueData();
	virtual void updateImpl();
protected:
	void changePosition(Vector3 newPosition) { position = newPosition; buildObjectModelMatrix(); }
	void setModel(Model* mr) { ptrToModel = mr; }
	const Matrix& getObjectModel() const { return objectModel; }
	Shader* getPtrToShader() const { return ptrToShader; }
	Model* getPtrToModel() const { return ptrToModel; }
	Vector3 getScale() { return scale; }
	Vector3 getObjectRotation() { return rotation; }
	const Vector3& getObjectPosition() const { return position; }
public:
	SceneObject(int id_, Vector3& position_, Vector3& rotation_, Vector3& scale_, Vector3& color_,
		Vector3& followingCamera_, Model* modelPtr, Shader* shaderPtr, const std::vector<Texture*> vTexturePtr, bool wired);
	SceneObject(int id_, Vector3& position_, Vector3& rotation_, Vector3& scale_, Vector3& color_,
		Vector3& followingCamera_, int modelId, int shaderId, const std::vector<int> vTextureIds, bool wired);
	void draw();
	void update();
};

