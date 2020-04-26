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
private:
	ResourceManager();
	~ResourceManager();
public:
	static ResourceManager & GetInstance();

	//maybe switch the use of shared_ptr over to the resourceManager managing the lifetimes?
	std::map<std::string, std::shared_ptr<ShaderProgram>> shaders;
	std::map<std::string, std::shared_ptr<Mesh>> meshes;
	std::map<std::string, std::shared_ptr<Material>> materials;
	std::map<std::string, std::shared_ptr<Texture>> textures;
	//std::map<std::string, std::shared_ptr<GfxObject>> renderObjects;//Combination of mesh and material

	void GetShader(const std::string & name);
	void GetMesh(const std::string & name);
	void GetMaterial(const std::string & name);
	void GetTexture(const std::string & name);

	void AddShader(std::shared_ptr<ShaderProgram> shader);
	void LoadMeshObj(const std::string & filepath);
	void LoadTexture2D(const std::string & filepath);
	void AddMaterial(std::shared_ptr<Material> material);
};