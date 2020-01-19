#include "Renderer.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

size_t GameObject::IDCounter = 0;

ObjectRenderer::ObjectRenderer()
{
}

ObjectRenderer::~ObjectRenderer()
{
}

void ObjectRenderer::AddObject(GameObject * obj)
{
	auto & shaderGroup = renderGroups[obj->material->shader.get()];
	std::unordered_map<Mesh*, std::vector<GameObject*>> & meshGroups = shaderGroup[obj->material];
	std::vector<GameObject*> & objects = meshGroups[obj->mesh];
	objects.push_back(obj);
}

void ObjectRenderer::RemoveObject(GameObject * obj)
{
	auto & shaderGroup = renderGroups[obj->material->shader.get()];
	std::unordered_map<Mesh*, std::vector<GameObject*>> & meshGroups = shaderGroup[obj->material];
	std::vector<GameObject*> & objects = meshGroups[obj->mesh];
	for (int i = 0; i < objects.size(); i++) {
		if (objects[i] == obj) {
			objects.erase(objects.begin() + i);
		}
	}
	if (objects.size() == 0) meshGroups.erase(obj->mesh);
	if (meshGroups.size() == 0) shaderGroup.erase(obj->material);
	if (shaderGroup.size() == 0) renderGroups.erase(obj->material->shader.get());
}

void ObjectRenderer::Draw()
{
	for (std::pair<ShaderProgram*, std::unordered_map<Material*, std::unordered_map<Mesh*, std::vector<GameObject*>>>>  shaderGroup : renderGroups)
	{
		shaderGroup.first->UseProgram();
		GLuint modelMatrixLocation = shaderGroup.first->GetUniformLocation("modelMatrix");
		GLuint normalMatrixLocation = shaderGroup.first->GetUniformLocation("modelNormalMatrix");

		for (std::pair<Material*, std::unordered_map<Mesh*, std::vector<GameObject*>>> meshGroup : shaderGroup.second) {
			meshGroup.first->ApplyProperties();
			
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

void ObjectRenderer::DrawOverrideMaterial(Material& material)
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

GameObject::~GameObject()
{
}

Transform & GameObject::GetTransform()
{
	return transform;
}
