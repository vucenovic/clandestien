#include "GameObject.h"

GameObject::~GameObject()
{
}

Transform & GameObject::GetTransform()
{
	return transform;
}