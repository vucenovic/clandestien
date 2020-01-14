#pragma once

#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include "Utils.h"
#include <memory>

/*
	Classes describing geometric objects in 2D
*/
namespace Physics {

	class AABB;
	class Circle;

	enum class ColliderType : char {AABB, CIRCLE};

	class Collider {
	public:
		inline virtual const ColliderType & getType() = 0;
		virtual AABB GetAABB() = 0;
		virtual AABB GetAABB(Transform transform) = 0;

		static void TransformCollider(Collider & collider, Transform transform);
	};

	class AABB : public Collider {
	public:
		glm::vec2 center;
		glm::vec2 min;
		glm::vec2 max;

		AABB(glm::vec2 min, glm::vec2 max);

		AABB GetAABB() final;
		AABB GetAABB(Transform transform) final;
		inline const ColliderType & getType() final { return ColliderType::AABB; };
	};

	class Circle : public Collider {
	public:
		float radius;
		glm::vec2 pos;

		Circle(float radius, glm::vec2 pos);

		AABB GetAABB() final;
		AABB GetAABB(Transform transform) final;
		inline const ColliderType & getType() final { return ColliderType::CIRCLE; };
	};
}