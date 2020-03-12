#pragma once

#include <map>
#include <string>
#include <memory>

#include "ShaderProgram.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

class ResourceManager
{
public:
	//maybe switch the use of shared_ptr over to the resourceManager managing the lifetimes?
	std::map<std::string, std::shared_ptr<ShaderProgram>> shaders;
	std::map<std::string, std::shared_ptr<Mesh>> meshes;
	std::map<std::string, std::shared_ptr<Material>> materials;
	std::map<std::string, std::shared_ptr<Texture>> textures;
	//std::map<std::string, std::shared_ptr<GfxObject>> renderObjects;//Combination of mesh and material

	//Frees all loaded Resources
	void Cleanup();
};