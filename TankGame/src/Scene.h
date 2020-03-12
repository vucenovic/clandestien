#pragma once

#include <unordered_map>
#include <map>
#include <string>
#include <vector>

#include "ShaderProgram.h"
#include "Material.h"
#include "Mesh.h"
//#include "GameObject"
#include "Renderer.h" //TODO move Gameobject into own file

//TODO (maybe): Maybe split into "Scene" and "SceneRenderer" classes?
class Scene
{
private:
	//BSP tree for all the transparent objects
	//BSPTree<GameObject> transparentObjects;

	//Fixed Depth Tree for all the opaque objects
	std::unordered_map<ShaderProgram *, std::unordered_map<Material*, std::unordered_map<Mesh*, std::vector<GameObject*>>>> renderGroups;

	//string map for searching objects by name
	std::map<std::string, GameObject> gameObjects;

private:
	void DrawOpaqueObjects();
	void DrawTransparentObjects();
public:

	void DrawScene(bool drawPortals);
};