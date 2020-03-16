#pragma once

#include "GameObject.h"
#include <array>
#include <map>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include<iostream>

/*
	BSPTree for transparent objects.
	Computes a binary tree by comparing position of centroids of game objects relative to hyperplanes that cut the scene in convex spaces containing one object at a time.
	Default splitter is at (0, 0, 0) upon initialization, if no splitter is defined upon calling the constructor.
*/

class BSPTree
{
	private:

		// Node

		struct BSPTreeNode {
			int value; // TODO: Exchange with value needed 
			struct BSPTreeNode *left;
			struct BSPTreeNode *right;
		};

		//Variables

		BSPTreeNode *root;
		std::map<GameObject*, glm::vec3> sceneData;
		glm::vec3 startingSplitter;

	public:

		// Constructors

		BSPTree();
		BSPTree(std::map<GameObject*, glm::vec3>);
		BSPTree(std::map<GameObject*, glm::vec3>, glm::vec3);
		
		// Functions

		void setSceneData(std::map<GameObject*, glm::vec3>);
		void setStartingSplitter(glm::vec3);
		void createTree();
		void addGameObject(GameObject* o, glm::vec3);
		void traverseInOrder();
		void traverseInOrder(BSPTreeNode *n);

};

