#pragma once
#include <PhysX/PxPhysicsAPI.h>
using namespace physx;

class GameSceneAggregateBuilder
{
	private:
		PxPhysics* physics;

	public:

		PxAggregate* gameSceneAggregate;

		void addStaticBox(PxTransform pos, PxBoxGeometry scale);

		GameSceneAggregateBuilder(PxU32 nActors, bool selfCollide, PxPhysics* physics);
};

