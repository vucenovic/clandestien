#pragma once
#include "Utils.h"
#include "Mesh.h"
#include <memory>

class Portal
{
public:
	Transform transform;
	Transform targetTransform;
	Mesh * portalMesh;

public:
	Portal();
	~Portal();

	glm::mat4 getOffsetMatrix() const;
	glm::vec4 getClipPlane() const;
};
