#include "Physics.h"

namespace Physics {
	AABB::AABB(glm::vec2 min, glm::vec2 max) : min(min), max(max), center((min+max) * 0.5f)
	{
	}

	AABB AABB::GetAABB()
	{
		return *this;
	}

	AABB AABB::GetAABB(Transform transform)
	{
		return AABB(min + glm::vec2(transform.position), max + glm::vec2(transform.position));
	}

	Circle::Circle(float radius, glm::vec2 pos) : radius(radius), pos(pos)
	{
	}

	AABB Circle::GetAABB()
	{
		return AABB(pos - radius, pos + radius);
	}

	AABB Circle::GetAABB(Transform transform)
	{
		return AABB(pos - radius + glm::vec2(transform.position), pos + radius + glm::vec2(transform.position));
	}

	void Collider::TransformCollider(Collider & collider, Transform transform)
	{
		ColliderType type = collider.getType();
		switch (type)
		{
		case Physics::ColliderType::AABB:
		{
			AABB & col = (AABB&)collider;
			col.center = glm::vec2(transform.position) + col.center;
			col.min = glm::vec2(transform.position) + col.min;
			col.max = glm::vec2(transform.position) + col.max;
		}
			break;
		case Physics::ColliderType::CIRCLE:
		{
			Circle & col = (Circle&)collider;
			col.pos = glm::vec2(transform.position) + col.pos;
		}
			break;
		default:
			break;
		}
	}
}
