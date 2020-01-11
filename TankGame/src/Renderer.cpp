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
	std::unordered_map<Mesh*, std::vector<GameObject*>> & meshGroups = renderGroups[obj->material];
	std::vector<GameObject*> & objects = meshGroups[obj->mesh];
	objects.push_back(obj);
}

void ObjectRenderer::RemoveObject(GameObject * obj)
{
	std::unordered_map<Mesh*, std::vector<GameObject*>> & meshGroups = renderGroups[obj->material];
	std::vector<GameObject*> & objects = meshGroups[obj->mesh];
	for (int i = 0; i < objects.size(); i++) {
		if (objects[i] == obj) {
			objects.erase(objects.begin() + i);
		}
	}
	if (objects.size() == 0) meshGroups.erase(obj->mesh);
	if (meshGroups.size() == 0) renderGroups.erase(obj->material);
}

void ObjectRenderer::Draw()
{
	for (std::pair<Material*, std::unordered_map<Mesh*, std::vector<GameObject*>>>  MeshGroups : renderGroups)
	{
		MeshGroups.first->Use();
		GLuint modelMatrixLocation = MeshGroups.first->shader->GetUniformLocation("modelMatrix");
		GLuint normalMatrixLocation = MeshGroups.first->shader->GetUniformLocation("modelNormalMatrix");
		for (std::pair<Mesh*, std::vector<GameObject*>> GameObjects : MeshGroups.second)
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

GameObject::~GameObject()
{
}

Transform & GameObject::GetTransform()
{
	return transform;
}
