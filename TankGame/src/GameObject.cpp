#include "GameObject.h"

size_t GameObject::IDCounter = 0;

GameObject::~GameObject()
{
}

Transform & GameObject::GetTransform()
{
	return transform;
}