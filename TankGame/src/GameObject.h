#pragma once

#include <string>

#include "Utils.h"
#include "Mesh.h"
#include "Material.h"

//unify Gameobjects with other types *maybe* *idunno*
enum class GameObjectType {
	Object,
	ParticleSystem,
	Light,
	Camera
};

class GameObject //TODO separate material and mesh from gameobject
{
private:
	Transform transform;
public:
	std::string name;//must be unique in a scene

	Mesh * mesh;
	Material * material;

	GameObject() : transform(Transform()), mesh(nullptr), material(nullptr) {};
	~GameObject();

	//virtual const GameObjectType & GetType() const { return GameObjectType::Object; }

	Transform & GetTransform();
};
