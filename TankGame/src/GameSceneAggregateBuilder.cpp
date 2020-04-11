#include "GameSceneAggregateBuilder.h"


void GameSceneAggregateBuilder::addStaticBox(PxTransform pos, PxBoxGeometry scale)
{
	PxMaterial* boxMaterial = this->physics->createMaterial(0.5f, 0.5f, 0.6f);	// create a material for the bounding box
	PxRigidStatic* gameSceneActor = this->physics->createRigidStatic(pos);	// create static body and transform via position output of blender script
	PxShape* aBoxShape = PxRigidActorExt::createExclusiveShape(*gameSceneActor,
		scale, *boxMaterial);
	gameSceneAggregate->addActor(*gameSceneActor);
}

GameSceneAggregateBuilder::GameSceneAggregateBuilder(PxU32 nActors, bool selfCollide, PxPhysics* physics)
{
	this->physics = physics;
	this->gameSceneAggregate = physics->createAggregate(nActors, selfCollide);
}
