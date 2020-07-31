#pragma once

#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "ShaderProgram.h"
#include "Material.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Portal.h"
#include "Camera.h"
#include "LightManager.h"

//TODO (maybe): Maybe split into "Scene" and "SceneRenderer" classes?
class Scene
{
private:
	//BSP tree for all the transparent objects
	//BSPTree<GameObject> transparentObjects;

	//Fixed Depth Tree for all the opaque objects
	std::unordered_map<ShaderProgram *, std::unordered_map<Material*, std::unordered_map<Mesh*, std::vector<GameObject*>>>> renderGroups;

	//string map for searching objects by name
	std::map<std::string, std::unique_ptr<GameObject>> gameObjects;
	LightManager& lm;
	
public:
	std::vector<Portal> renderPortals;
	ShaderProgram * portalHoldoutShader;
	ShaderProgram * depthResetSS;
	UniformBuffer * viewDataBuffer;
	GLuint SSrectVAOId;

	Camera * activeCamera;

public:
	Scene(LightManager& lm);
	void DrawOpaqueObjects(const Material & material);
	void DrawOpaqueObjects();
	void DrawTransparentObjects();
	void RenderPortal(const Portal * portal);

public:
	bool AddObject(std::unique_ptr<GameObject> & gameobject);
	void addLights(LightManager& lm) { this->lm = lm; };
	LightManager& getLightManager() { return lm; };
	GameObject * GetObject(const std::string & name);
	void RemoveObject(const std::string & name);

	void DrawScene(bool drawPortals);

public:
	void UpdateScene();

};