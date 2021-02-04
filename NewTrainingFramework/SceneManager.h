#pragma once
#include "rapidxml.hpp"
#include "../Utilities/utilities.h"
#include "Camera.h"
#include "SceneObject.h"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <tuple>

class SceneManager
{
private:
	Shader* debugAxisShader;
	static SceneManager* instance;
	unsigned int debugAxisVBOid;
	unsigned int debugAxisIBOid;
	std::vector<std::shared_ptr<SceneObject>> objects;
	std::map<int, std::unique_ptr<Camera>> cameras;
	int activeCamera;
	Vector3 backgroundColor;
	std::string gameName;
	int viewPortWidth, viewPortHeight;
	bool fullScreen;
	float rFog, RFog;
	unsigned int fogTextureId;
	std::map<int, std::tuple<void (Camera::*) (Camera::Axis, Camera::Direction), Camera::Axis, Camera::Direction>> controls;
	char debugModeKey;
	char switchActiveCameraKey;
	bool isInDebugMode;

public:
	static SceneManager* getInstance();
	void Init(const std::string& fileName);
	void Draw();
	void Update();
	Camera* getActiveCamera() { return cameras[activeCamera].get(); }
	void setActiveCameraDeltaTime(GLfloat deltaTime) { cameras[activeCamera]->setDeltaTime(deltaTime); };
	unsigned int getFogTextureId() const { return fogTextureId; }
	float getR() const { return RFog; }
	float getr() const { return rFog; }
	bool getDebugModeStatus() { return isInDebugMode; }
	void RegisterKey(char key);
	const std::string& getGameName() const { return gameName; }
	int getViewPortWidth() const { return viewPortWidth; }
	int getViewPortHeight() const { return viewPortHeight; }
	void drawDebugAxis(Matrix& modelMatrix);
private:
	SceneManager() = default;
	SceneManager(const SceneManager&);

	void readGameProperties(rapidxml::xml_document<> & doc);

	void readControls(rapidxml::xml_node<> * controlNode);
	auto getActionControlTuple(char* action);
	int getControlKey(char* key);

	void readCameras(rapidxml::xml_document<> & doc);
	Camera* readCamera(rapidxml::xml_node<> * ptrToCameraNode);

	void readObjects(rapidxml::xml_document<> & doc);
	SceneObject* readObject(rapidxml::xml_node<> * ptrToObjectNode);
	void readObjectTrajectory(SceneObject* newObject, rapidxml::xml_node<> * objectNode);

	void readVectorElement(rapidxml::xml_node<> * ptrToVector3Node, Vector3 & element, const char* components);
	void readFogAttributes(rapidxml::xml_node<> * ptrToFogAttributes);
	char* cutTextureFace(char* bigTexture, int xOffset, int yOffset, int bytespp, int bigWidth, int faceWidth, int faceHeight);

	void operateActiveCamera(char key);
	void switchActiveCamera();
	void loadDebugAxisModel();
};														   