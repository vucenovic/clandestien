#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

ResourceManager & ResourceManager::GetInstance()
{
	static ResourceManager instance;
	return instance;
}

void ResourceManager::Clear()
{
	materials.clear();
	meshes.clear();
	shaders.clear();
	textures.clear();
}
