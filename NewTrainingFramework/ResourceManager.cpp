#include "stdafx.h"
#include "ResourceManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager* ResourceManager::getInstance() {
	if (instance == nullptr) {
		instance = new ResourceManager();
	}
	return instance;
}

void ResourceManager::Init(const std::string& fileName) {
	std::unique_ptr<rapidxml::xml_document<>> doc(new rapidxml::xml_document<>());
	std::ifstream fin(fileName);

	if (!fin.is_open()) {
		std::string errorMessage = "File " + fileName + " could not be opened for parsing!";
		throw std::exception(errorMessage.c_str());
	}

	std::stringstream buffer;
	buffer << fin.rdbuf();
	fin.close();
	std::string content(buffer.str());
	doc->parse<0>(&content[0]);
	readResources(*doc);
}

// TODO : try to improve resource reading functions
void ResourceManager::readResources(rapidxml::xml_document<>& xml_root) {
	auto readModelResource = [](rapidxml::xml_node<>* xmlResPtr, ModelResource* mr) {
		mr->fileName = xmlResPtr->first_node()->value(); };

	auto readShaderResource = [](rapidxml::xml_node<>* xmlResPtr, ShaderResource* sr) {
		sr->vertexShader = xmlResPtr->first_node("vs")->value();
		sr->fragmentShader = xmlResPtr->first_node("fs")->value(); };

	auto readTextureResource = [](rapidxml::xml_node<>* xmlResPtr, TextureResource* tr) {
		tr->fileName = xmlResPtr->first_node("file")->value();
		tr->type = TextureResource::getType(xmlResPtr->first_attribute("type")->value());
		tr->minFilter = TextureResource::getFilter(xmlResPtr->first_node("min_filter")->value());
		tr->magFilter = TextureResource::getFilter(xmlResPtr->first_node("mag_filter")->value());
		tr->wrapS = TextureResource::getWrap(xmlResPtr->first_node("wrap_s")->value());
		tr->wrapT = TextureResource::getWrap(xmlResPtr->first_node("wrap_t")->value()); };

	rapidxml::xml_node<>* pRoot = xml_root.first_node();

	rapidxml::xml_node<>* resourcesPtr = pRoot->first_node("models");
	readResourceData(resourcesPtr, modelResources, readModelResource);

	resourcesPtr = pRoot->first_node("shaders");
	readResourceData(resourcesPtr, shaderResources, readShaderResource);

	resourcesPtr = pRoot->first_node("textures");
	readResourceData(resourcesPtr, textureResources, readTextureResource);
}

// TODO : try to improve resource reading functions
template <class T, class U>
void ResourceManager::readResourceData(rapidxml::xml_node<>* resourcesPtr, std::map<int, std::unique_ptr<T>>& resourcesContainer, U resourceReadingFunc)
{
	rapidxml::xml_node<>* folderNode = resourcesPtr->first_node("folder");
	while (folderNode) {
		std::string path = folderNode->first_attribute("path")->value();
		for (rapidxml::xml_node<>* res = folderNode->first_node(); res; res = res->next_sibling()) {
			int id = atoi(res->first_attribute()->value());
			T* newResPtr = new T();
			newResPtr->path = path;
			resourceReadingFunc(res, newResPtr);
			resourcesContainer.insert(std::pair<int, std::unique_ptr<T>>(id, std::unique_ptr<T>(newResPtr)));
		}
		folderNode = folderNode->next_sibling();
	}
}