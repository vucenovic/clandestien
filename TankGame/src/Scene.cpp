#include "Scene.h"

bool Scene::AddObject(std::unique_ptr<GameObject> & obj)
{
	if (gameObjects.find(obj->name) == gameObjects.end()) {
		auto & shaderGroup = renderGroups[obj->material->shader.get()];
		std::unordered_map<Mesh*, std::vector<GameObject*>> & meshGroups = shaderGroup[obj->material];
		std::vector<GameObject*> & objects = meshGroups[obj->mesh];
		objects.push_back(obj.get());
		gameObjects[obj->name] = std::move(obj);
		return true;
	}
	return false; //GameObject already exists
}

GameObject * Scene::GetObject(const std::string & name)
{
	if (gameObjects.find(name) == gameObjects.end()) {
		return nullptr;
	}
	return gameObjects[name].get();
}

void Scene::RemoveObject(const std::string & name)
{
	auto & obj = gameObjects[name];
	auto & shaderGroup = renderGroups[obj->material->shader.get()];
	std::unordered_map<Mesh*, std::vector<GameObject*>> & meshGroups = shaderGroup[obj->material];
	std::vector<GameObject*> & objects = meshGroups[obj->mesh];
	for (int i = 0; i < objects.size(); i++) {
		if (objects[i] == obj.get()) {
			objects.erase(objects.begin() + i);
		}
	}
	if (objects.size() == 0) meshGroups.erase(obj->mesh);
	if (meshGroups.size() == 0) shaderGroup.erase(obj->material);
	if (shaderGroup.size() == 0) renderGroups.erase(obj->material->shader.get());
	gameObjects.erase(name);
}

void Scene::DrawOpaqueObjects(const Material & material)
{
	material.Use();
	GLuint modelMatrixLocation = material.shader->GetUniformLocation("modelMatrix");
	GLuint normalMatrixLocation = material.shader->GetUniformLocation("modelNormalMatrix");

	for (std::pair<ShaderProgram*, std::unordered_map<Material*, std::unordered_map<Mesh*, std::vector<GameObject*>>>> shaderGroup : renderGroups)
	{
		for (std::pair<Material*, std::unordered_map<Mesh*, std::vector<GameObject*>>> meshGroup : shaderGroup.second) {
			for (std::pair<Mesh*, std::vector<GameObject*>> GameObjects : meshGroup.second)
			{
				GameObjects.first->Bind();
				for (GameObject* gameObject : GameObjects.second)
				{
					glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(gameObject->GetTransform().ToMatrix()));
					glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(gameObject->GetTransform().ToNormalMatrix()));
					GameObjects.first->Draw();
				}
			}
		}
	}
}

void Scene::DrawOpaqueObjects()
{
	for (auto shaderGroup : renderGroups)
	{
		shaderGroup.first->UseProgram();
		GLuint modelMatrixLocation = shaderGroup.first->GetUniformLocation("modelMatrix");
		GLuint normalMatrixLocation = shaderGroup.first->GetUniformLocation("modelNormalMatrix");

		for (auto meshGroup : shaderGroup.second) {
			meshGroup.first->ApplyProperties();

			for (auto GameObjects : meshGroup.second)
			{
				GameObjects.first->Bind();
				for (GameObject* gameObject : GameObjects.second)
				{
					glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(gameObject->GetTransform().ToMatrix()));
					glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(gameObject->GetTransform().ToNormalMatrix()));
					GameObjects.first->Draw();
				}
			}
		}
	}
}

void Scene::DrawTransparentObjects()
{
	
}

void Scene::RenderPortal(const Portal * portal)
{
	glEnable(GL_STENCIL_TEST);
	glClear(GL_STENCIL_BUFFER_BIT);

	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	//bind portalHoldout shader

	portal->portalMesh->BindAndDraw();

	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	//bind depthResetScreenspace shader
	//bind and draw screenspace rectangle

	glEnable(GL_DEPTH_TEST);

	//SetViewParameters
	glm::mat4 view = portal->getOffsetMatrix() * activeCamera->GetTransform().ToInverseMatrix();
	Camera::SetViewParameters(*viewDataBuffer, view, activeCamera->getProjectionMatrix());

	DrawScene(false);
	glClear(GL_STENCIL_BUFFER_BIT);
	glDisable(GL_STENCIL_TEST);
}

void Scene::DrawScene(bool drawPortals)
{
	//Draw WorldPortals between Opaque phase and transparent phase
	//use stencil buffer for masking drawing and an oblique projection

	//render portal to stencil buffer
	//render screen quad with only stencil testing and write depth to be the far plane (to reset depth buffer in the portal)
	//rerender scene from other perspective with stencil and depth testing
	//?
	//profit
	activeCamera->UseCamera(*viewDataBuffer);
	DrawOpaqueObjects();
	if (drawPortals) {
		for(Portal portal : renderPortals){
			RenderPortal(&portal);
		}
	}
	DrawTransparentObjects();
}
