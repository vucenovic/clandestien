#include "Key.h"

void Key::interact(physx::PxRigidBody * actor, physx:: PxRigidBody * invoker, physx::PxRaycastBuffer & hit, GameLogic & gameLogic)
{
	gameLogic.setCameraState(1);

	// unhexing logic TODO: move into gameLogic

	/*if (first && second && third && fourth) {
		actor->detachShape(*shape);
		gameLogic.getPxScene()->removeActor(*actor);
		gameLogic.getScene().RemoveObject("Key");
		Inventory::instance().addKey();
	}*/
	

}


