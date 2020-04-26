#include "ResourceManager.h"

ResourceManager & ResourceManager::GetInstance()
{
	static ResourceManager instance;
	return instance;
}
