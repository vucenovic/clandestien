#pragma once

#include "Utils.h"
#include "Mesh.h"
#include "Material.h"

//unify Gameobjects with other types *maybe* *idunno*
enum class GameObjectType {
	Object,
	Camera
};

class GameObject //TODO separate material and mesh from gameobject
{
private:
	static size_t IDCounter;
	size_t ID;
	Transform transform;
public:
	Mesh * mesh;
	Material * material;

	GameObject() : ID(IDCounter++), transform(Transform()), mesh(nullptr), material(nullptr) {};
	~GameObject();

	//virtual const GameObjectType & GetType() const { return GameObjectType::Object; }

	Transform & GetTransform();
};
