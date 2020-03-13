#pragma once

#include <unordered_map>
#include <vector>

#include "ShaderProgram.h"
#include "Material.h"
#include "Mesh.h"
#include "Utils.h"
#include "Particles.h"
#include "GameObject.h"

class GfxObject
{
public:
	Mesh * mesh;
	Material * material;
};

class MasterRenderer
{
private:
	std::unordered_map<ShaderProgram *, std::unordered_map<Material*, std::unordered_map<Mesh*, std::vector<GameObject*>>>> renderGroups;//Improve sort by render pass
public:
	MasterRenderer();
	~MasterRenderer();

	void AddObject(GameObject * obj);
	void RemoveObject(GameObject * obj);

	void Draw();
	void DrawOverrideMaterial(Material& material);
};