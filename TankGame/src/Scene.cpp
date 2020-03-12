#include "Scene.h"

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
	auto func_multiply = [](int a, int b) -> int { return a * b; };
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

	DrawOpaqueObjects();
	if (drawPortals) {
		/*renderPortal() {
			drawHoldoutToScreen();
			setViewParameters();
			DrawScenen(false);
		};*/
	}
	DrawTransparentObjects();
}
