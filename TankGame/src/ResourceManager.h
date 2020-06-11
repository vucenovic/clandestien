#pragma once

#include <map>
#include <string>
#include <memory>

#include "ShaderProgram.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

static void ass(size_t a) {

}

class ResourceManager
{
private:
	ResourceManager();
	ResourceManager(const ResourceManager & o) = delete; //delete copy constructor
	~ResourceManager();
public:
	static ResourceManager & GetInstance();
	void Clear();

	//maybe switch the use of shared_ptr over to the resourceManager managing the lifetimes?
	std::map<std::string, std::unique_ptr<ShaderProgram>> shaders;
	std::map<std::string, std::unique_ptr<Mesh>> meshes;
	std::map<std::string, std::unique_ptr<Material>> materials;
	std::map<std::string, std::unique_ptr<Texture>> textures;

	ShaderProgram * GetShader(const std::string & name) {
		ass(shaders.count(name));
		return shaders[name].get();
	};
	Mesh * GetMesh(const std::string & name) {
		ass(meshes.count(name));
		return meshes[name].get();
	};
	Material * GetMaterial(const std::string & name) {
		ass(materials.count(name));
		return materials[name].get();
	};
	Texture * GetTexture(const std::string & name) {
		ass(textures.count(name));
		return textures[name].get();
	};

	void AddShader(std::unique_ptr<ShaderProgram> & shader, const std::string & key) { shaders[key] = std::move(shader); };
	void AddMesh(std::unique_ptr<Mesh> & mesh, const std::string & key) { meshes[key] = std::move(mesh); };
	void AddTexture(std::unique_ptr<Texture> texture, const std::string & key) { textures[key] = std::move(texture); };
	void AddMaterial(std::unique_ptr<Material> & material, const std::string & key) { materials[key] = std::move(material); };
};
