#pragma once

#include <unordered_map>
#include <vector>

#include "ShaderProgram.h"
#include "Material.h"
#include "Geometry.h"
#include "MyUtils.h"

class GameObject
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

	Transform & GetTransform();
};

class ObjectRenderer
{
private:
	std::unordered_map<Material*, std::unordered_map<Mesh*, std::vector<GameObject*>>> renderGroups;//improve grouping, sort by shader first, fucking somehow And sort by "renderLayer"
public:
	ObjectRenderer();
	~ObjectRenderer();

	void AddObject(GameObject * obj);
	void RemoveObject(GameObject * obj);

	void Draw();
};