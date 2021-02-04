#pragma once
#include "rapidxml.hpp"
#include "ModelResource.h"
#include "ShaderResource.h"
#include "TextureResource.h"
#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include <map>
#include <memory>
#include <type_traits>
#include <iostream>

class ResourceManager
{
private:
	static ResourceManager* instance;

	std::map<int, std::unique_ptr<ModelResource>> modelResources;
	std::map<int, std::unique_ptr<ShaderResource>> shaderResources;
	std::map<int, std::unique_ptr<TextureResource>> textureResources;

	std::map<int, std::unique_ptr<Model>> models;
	std::map<int, std::unique_ptr<Shader>> shaders;
	std::map<int, std::unique_ptr<Texture>> textures;

public:
	static const int GENERATED_MODEL_ID = -1;
	static ResourceManager* getInstance();
	void Init(const std::string& fileName);
	void addGeneratedModel(Model* mr) { models.insert(std::pair<int, std::unique_ptr<Model>>(GENERATED_MODEL_ID, mr)); }
	template <class T>
	T * Load(int id);
	template <class T>
	T * GetResource(int id);

private:
	ResourceManager() = default;
	ResourceManager(const ResourceManager&);
	void readResources(rapidxml::xml_document<> & xml_root);
	template <class T, class U>
	void readResourceData(rapidxml::xml_node<> * resorcesPtr, std::map<int, std::unique_ptr<T>> & resourceContainer, U resourceReadingFunc);
};

template <class T>
T* ResourceManager::GetResource(int id)
{
	std::map<int, std::unique_ptr<T>>* ptrToResourceMap;

	if constexpr (std::is_same<ModelResource, T>::value == true)
		ptrToResourceMap = &modelResources;
	else if constexpr (std::is_same<TextureResource, T>::value == true)
		ptrToResourceMap = &textureResources;
	else if constexpr (std::is_same<ShaderResource, T>::value == true)
		ptrToResourceMap = &shaderResources;
	if (ptrToResourceMap->find(id) != ptrToResourceMap->end())
		return (*ptrToResourceMap)[id].get();
	else return nullptr;
}

template <class T>
T* ResourceManager::Load(int id)
{
	T* ptr;
	std::map<int, std::unique_ptr<T::resourceType>>* resourcesMapPtr;
	std::map<int, std::unique_ptr<T>>* loadedMapPtr;

	// locate the maps holding the resource
	// coresponding to given id
	if constexpr (std::is_same<T, Model>::value == true) {
		resourcesMapPtr = &modelResources;
		loadedMapPtr = &models;
	}
	else if constexpr (std::is_same<T, Shader>::value == true) {
		resourcesMapPtr = &shaderResources;
		loadedMapPtr = &shaders;
	}
	else if constexpr (std::is_same<T, Texture>::value == true) {
		resourcesMapPtr = &textureResources;
		loadedMapPtr = &textures;
	}

	// give ptr a value
	if (loadedMapPtr->find(id) != loadedMapPtr->end())
		ptr = (*loadedMapPtr)[id].get();
	else {
		ptr = new T((*resourcesMapPtr)[id].get());
		ptr->load();
		loadedMapPtr->insert(std::pair<int, std::unique_ptr<T>>(id, std::unique_ptr<T>(ptr)));
	}

	return ptr;
}