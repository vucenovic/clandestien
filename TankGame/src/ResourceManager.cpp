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
