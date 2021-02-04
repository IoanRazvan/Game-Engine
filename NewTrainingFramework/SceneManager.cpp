#include "stdafx.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Fire.h"
#include "SkyBox.h"
#include "Terrain.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <array>
#include <iostream>

SceneManager* SceneManager::instance = nullptr;

SceneManager* SceneManager::getInstance()
{
	if (instance == nullptr)
		instance = new SceneManager();
	return instance;
}

void SceneManager::Init(const std::string& fileName)
{
	std::unique_ptr<rapidxml::xml_document<>> doc(new rapidxml::xml_document<>);
	std::ifstream fin(fileName);

	if (!fin.is_open()) {
		std::string errorMessage = "File " + fileName + " could not be opened for parsing!";
		throw std::exception(errorMessage.c_str());
	}

	std::stringstream sstrIn;
	sstrIn << fin.rdbuf();
	fin.close();
	std::string xmlContent(sstrIn.str());

	doc->parse<0>(&xmlContent[0]);
	readGameProperties(*doc);
	readCameras(*doc);
	readObjects(*doc);
}

void SceneManager::RegisterKey(char key)
{
	if (key == debugModeKey)
		isInDebugMode = !isInDebugMode;
	else if (key == switchActiveCameraKey)
		switchActiveCamera();
	else
		operateActiveCamera(key);
}

void SceneManager::drawDebugAxis(Matrix& modelMatrix)
{	
	glBindBuffer(GL_ARRAY_BUFFER, debugAxisVBOid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugAxisIBOid);
	debugAxisShader->bind();
	
	if (debugAxisShader->getAttrib("a_posL") != -1) {
		glEnableVertexAttribArray(debugAxisShader->getAttrib("a_posL"));
		glVertexAttribPointer(debugAxisShader->getAttrib("a_posL"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<const void*>(0));
	}

	if (debugAxisShader->getAttrib("a_color") != -1) {
		glEnableVertexAttribArray(debugAxisShader->getAttrib("a_color"));
		glVertexAttribPointer(debugAxisShader->getAttrib("a_color"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<const void*>(3 * sizeof(float)));
	}

	if (debugAxisShader->getUniform("u_matrix") != -1) {
		glUniformMatrix4fv(debugAxisShader->getUniform("u_matrix"), 1, GL_FALSE, &(modelMatrix * cameras[activeCamera]->getMVP()).m[0][0]);
	}

	glDrawElements(GL_LINES, 6, GL_UNSIGNED_SHORT, 0);
}

void SceneManager::readGameProperties(rapidxml::xml_document<>& doc)
{
	rapidxml::xml_node<>* ptrToRootNode = doc.first_node();

	if (rapidxml::xml_node<>* gameNamePtr = ptrToRootNode->first_node("gameName"))
		gameName = gameNamePtr->value();

	if (rapidxml::xml_node<>* backgroundColorPtr = ptrToRootNode->first_node("backgroundColor"))
		readVectorElement(backgroundColorPtr, backgroundColor, "r\0g\0b\0");

	if (rapidxml::xml_node<>* screenSizePtr = ptrToRootNode->first_node("defualtScreensSize")) {
		if (screenSizePtr->first_node("fullscreen"))
			fullScreen = true;
		else {
			viewPortHeight = atoi(screenSizePtr->first_node("width")->value());
			viewPortWidth = atoi(screenSizePtr->first_node("height")->value());
		}
	}
	else {
		viewPortHeight = 720;
		viewPortWidth = 960;
	}

	if (rapidxml::xml_node<>* fogPropNode = ptrToRootNode->first_node("fogProperties"))
		readFogAttributes(fogPropNode);

	if (rapidxml::xml_node<>* gameControlsPtr = ptrToRootNode->first_node("controls"))
		readControls(gameControlsPtr->first_node("control"));

	if (rapidxml::xml_node<>* debugModePtr = ptrToRootNode->first_node("debugModeKey")) {
		debugModeKey = debugModePtr->value()[0];
		isInDebugMode = false;
	}

	if (rapidxml::xml_node<>* debugAxisShaderPtr = ptrToRootNode->first_node("debugAxisShader")) {
		debugAxisShader = ResourceManager::getInstance()->Load<Shader>(atoi(debugAxisShaderPtr->value()));
	}
	loadDebugAxisModel();

	if (rapidxml::xml_node<>* switchCameraPtr = ptrToRootNode->first_node("switchActiveCameraKey"))
		switchActiveCameraKey = switchCameraPtr->value()[0];
}

int SceneManager::getControlKey(char* action)
{
	if (strlen(action) == 1)
		return action[0];
	else if (strcmp(action, "VK_UP") == 0)
		return VK_UP;
	else if (strcmp(action, "VK_DOWN") == 0)
		return VK_DOWN;
	else if (strcmp(action, "VK_LEFT") == 0)
		return VK_LEFT;
	else if (strcmp(action, "VK_RIGHT") == 0)
		return VK_RIGHT;
	else return -1;
};

auto SceneManager::getActionControlTuple(char* action)
{
	void (Camera:: * functionPtr) (Camera::Axis, Camera::Direction);
	Camera::Axis arg1; Camera::Direction arg2;
	if (strstr(action, "MOVE"))
		functionPtr = &Camera::translate;
	else
		functionPtr = &Camera::rotate;
	if (strstr(action, "POSITIVE"))
		arg2 = Camera::Direction::POSITIVE;
	else
		arg2 = Camera::Direction::NEGATIVE;
	if (strstr(action, "X"))
		arg1 = Camera::Axis::X;
	else if (strstr(action, "Y"))
		arg1 = Camera::Axis::Y;
	else
		arg1 = Camera::Axis::Z;
	return std::make_tuple(functionPtr, arg1, arg2);
}

void SceneManager::readControls(rapidxml::xml_node<>* ptrToControlNode)
{
	char key;
	char* action;
	do {
		key = getControlKey(ptrToControlNode->first_node("key")->value());
		action = ptrToControlNode->first_node("action")->value();
		controls.insert(std::pair<unsigned char, std::tuple<void (Camera::*) (Camera::Axis, Camera::Direction), Camera::Axis, Camera::Direction>>(key, getActionControlTuple(action)));
		ptrToControlNode = ptrToControlNode->next_sibling();
	} while (ptrToControlNode);
}


void SceneManager::readCameras(rapidxml::xml_document<>& doc)
{
	rapidxml::xml_node<>* ptrToRootNode = doc.first_node();
	rapidxml::xml_node<>* ptrToCameraNode;

	activeCamera = atoi(ptrToRootNode->first_node("activeCamera")->value());

	ptrToCameraNode = ptrToRootNode->first_node("cameras")->first_node("camera");

	int id;
	do {
		id = atoi(ptrToCameraNode->first_attribute("id")->value());
		cameras.insert(std::pair<int, std::unique_ptr<Camera>>(id, readCamera(ptrToCameraNode)));
		ptrToCameraNode = ptrToCameraNode->next_sibling();
	} while (ptrToCameraNode);
}


Camera* SceneManager::readCamera(rapidxml::xml_node<>* ptrToCameraNode)
{
	Vector3 position, target, up;
	GLfloat translationSpeed, rotationSpeed, fov, nearPlane, farPlane;

	// reading vector attributes of camera
	readVectorElement(ptrToCameraNode->first_node("position"), position, "x\0y\0z\0");
	readVectorElement(ptrToCameraNode->first_node("target"), target, "x\0y\0z\0");
	readVectorElement(ptrToCameraNode->first_node("up"), up, "x\0y\0z\0");

	// reading scalar attributes of camera
	translationSpeed = atof(ptrToCameraNode->first_node("translationSpeed")->value());
	rotationSpeed = atof(ptrToCameraNode->first_node("rotationSpeed")->value());
	nearPlane = atof(ptrToCameraNode->first_node("near")->value());
	farPlane = atof(ptrToCameraNode->first_node("far")->value());
	fov = atof(ptrToCameraNode->first_node("fov")->value());

	return new Camera(position, target, translationSpeed, rotationSpeed, nearPlane, farPlane, fov, 0.0f);
}

void SceneManager::readObjects(rapidxml::xml_document<>& doc)
{
	rapidxml::xml_node<>* ptrToRootNode = doc.first_node();
	rapidxml::xml_node<>* ptrToObjectNode;

	ptrToObjectNode = ptrToRootNode->first_node("objects")->first_node("object");
	do {
		objects.push_back(std::shared_ptr<SceneObject>(readObject(ptrToObjectNode)));
		ptrToObjectNode = ptrToObjectNode->next_sibling();
	} while (ptrToObjectNode);
}

// read data of current object node
SceneObject* SceneManager::readObject(rapidxml::xml_node<>* ptrToObjectNode)
{
	rapidxml::xml_node<>* ptrToOptionalAttribNode;
	Vector3 color, position, rotation, scale, followingCamera = Vector3(0.0f, 0.0f, 0.0f);
	std::vector<int> textures;
	int model, shader, id;
	bool wired = false;

	id = atoi(ptrToObjectNode->first_attribute("id")->value());
	readVectorElement(ptrToObjectNode->first_node("position"), position, "x\0y\0z\0");
	readVectorElement(ptrToObjectNode->first_node("scale"), scale, "x\0y\0z\0");
	readVectorElement(ptrToObjectNode->first_node("rotation"), rotation, "x\0y\0z\0");

	if (rapidxml::xml_node<>* followingCameraNode = ptrToObjectNode->first_node("followingCamera")) {
		followingCamera.x = followingCameraNode->first_node("ox") ? 1.0 : 0.0;
		followingCamera.y = followingCameraNode->first_node("oy") ? 1.0 : 0.0;
		followingCamera.z = followingCameraNode->first_node("oz") ? 1.0 : 0.0;

	}

	shader = atoi(ptrToObjectNode->first_node("shader")->value());

	if (rapidxml::xml_node<>* colorNode = ptrToObjectNode->first_node("color"))
		readVectorElement(colorNode, color, "r\0g\0b\0");

	if (rapidxml::xml_node<>* texturesNode = ptrToObjectNode->first_node("textures")) {
		texturesNode = texturesNode->first_node("texture");
		int texture;
		do {
			texture = atoi(texturesNode->first_attribute("id")->value());
			textures.push_back(texture);
			texturesNode = texturesNode->next_sibling();
		} while (texturesNode);
	}

	if (ptrToObjectNode->first_node("wired"))
		wired = true;

	const char* type = ptrToObjectNode->first_node("type")->value();

	if (strcmp(type, "normal") == 0) {
		model = atoi(ptrToObjectNode->first_node("model")->value());
		return  new SceneObject(id, position, rotation, scale, color, followingCamera, model, shader, textures, wired);
		//readObjectTrajectory(newObj, ptrToObjectNode);
	}
	else if (strcmp(type, "terrain") == 0) {
		int cells = atoi(ptrToObjectNode->first_node("cells")->value());
		int dim_cell = atoi(ptrToObjectNode->first_node("cell_dim")->value());
		int offsetY = atoi(ptrToObjectNode->first_node("offsetY")->value());
		Vector3 heights;
		readVectorElement(ptrToObjectNode->first_node("heights"), heights, "r\0g\0b\0");
		return new Terrain(cells, dim_cell, offsetY, heights, id, position, rotation, scale, color, shader, textures, wired);
	}
	else if (strcmp(type, "skybox") == 0) {
		model = atoi(ptrToObjectNode->first_node("model")->value());
		return new SkyBox(id, position, rotation, scale, color, followingCamera, model, shader, textures, wired);
	}

	else if (strcmp(type, "fire") == 0) {
		model = atoi(ptrToObjectNode->first_node("model")->value());
		float dispMax = atof(ptrToObjectNode->first_node("dispMax")->value());
		return new Fire(id, position, rotation, scale, color, model, shader, textures, wired, dispMax);
	}
	else
		return nullptr;
}

// read trajectory for an object
void SceneManager::readObjectTrajectory(SceneObject* newObject, rapidxml::xml_node<> * objectNode) {
}

void SceneManager::readVectorElement(rapidxml::xml_node<>* ptrToVector3Node, Vector3& element, const char* components)
{
	element.x = atof(ptrToVector3Node->first_node(components)->value());
	element.y = atof(ptrToVector3Node->first_node(components + 2)->value());
	element.z = atof(ptrToVector3Node->first_node(components + 4)->value());
}

char* SceneManager::cutTextureFace(char* bigTexture, int xOffset, int yOffset, int bytespp, int bigWidth, int faceWidth, int faceHeight) {
	char* ptr = bigTexture + yOffset * bytespp * bigWidth + xOffset * bytespp;
	char* cutTexture = new char[bytespp * faceWidth * faceHeight];

	for (int i = 0; i < faceHeight; i++) {
		memcpy(cutTexture + i * faceWidth * bytespp, ptr, faceWidth * bytespp);
		ptr += bytespp * bigWidth;
	}

	return cutTexture;
}

void SceneManager::readFogAttributes(rapidxml::xml_node<>* ptrToFogAttributes)
{
	rFog = atoi(ptrToFogAttributes->first_node("r")->value());
	RFog = atoi(ptrToFogAttributes->first_node("R")->value());
	int textureId = atoi(ptrToFogAttributes->first_node("cube_texture")->first_attribute("id")->value());
	TextureResource* fogTexture = ResourceManager::getInstance()->GetResource<TextureResource>(textureId);
	if (fogTexture == nullptr) {
		std::cerr << "Unknown texture was specified!";
		exit(EXIT_FAILURE);
	}

	else if (fogTexture->type != GL_TEXTURE_CUBE_MAP) {
		std::cerr << "The specified texture is not a cube map texture!";
		exit(EXIT_FAILURE);
	}

	std::string completeTextureFileName = fogTexture->path + fogTexture->fileName;
	int width, height, bpp;
	std::unique_ptr<char> bigTexture = std::unique_ptr<char>(LoadTGA(completeTextureFileName.c_str(), &width, &height, &bpp));
	std::unique_ptr<char> faceTexture = std::unique_ptr<char>(cutTextureFace(bigTexture.get(), width / 4, 0, bpp / 8, width, width / 4, height / 3));

	glGenTextures(1, &fogTextureId);
	glBindTexture(GL_TEXTURE_2D, fogTextureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, fogTexture->minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, fogTexture->magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, fogTexture->wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, fogTexture->wrapT);

	glTexImage2D(GL_TEXTURE_2D, 0, bpp == 32 ? GL_RGBA : GL_RGB, width / 4, height / 3, 0, bpp == 32 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, faceTexture.get());
}


void SceneManager::operateActiveCamera(char key)
{
	if (controls.find(key) != controls.end()) {
		auto fcPtr = std::get<void (Camera::*) (Camera::Axis, Camera::Direction)>(controls[key]);
		auto arg1 = std::get<Camera::Axis>(controls[key]);
		auto arg2 = std::get<Camera::Direction>(controls[key]);
		(cameras[activeCamera].get()->*fcPtr)(arg1, arg2);
	}
}

void SceneManager::Draw()
{
	for (auto i : objects) {
		if (i != nullptr)
			i->draw();
	}
}

void SceneManager::Update()
{
	for (auto i : objects)
		if (i != nullptr) {
			i->update();
		}
}

void SceneManager::switchActiveCamera() {
	auto it = cameras.find(activeCamera);
	it++;
	if (it == cameras.end())
		it = cameras.begin();
	activeCamera = it->first;
}

void SceneManager::loadDebugAxisModel()
{
	std::array<float, 36> vertices{ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
									80.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
									0.0f, 80.0f, 0.0f, 0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 80.0f, 0.0f, 0.0f, 1.0f };
	std::array<unsigned short, 6> indices{ 0, 3, 1, 4, 2, 5 };

	glGenBuffers(1, &debugAxisVBOid);
	glGenBuffers(1, &debugAxisIBOid);

	glBindBuffer(GL_ARRAY_BUFFER, debugAxisVBOid);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugAxisIBOid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
}
