#pragma once

#include "GameObject.h"
#include <array>
#include <vector>
#include <map>
#include <GL\glew.h>
#include <glm\glm.hpp>

/*
BSPTree for transparent objects.
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

		BSPTreeNode root;
		BSPTreeNode rightChild;
		BSPTreeNode leftChild;
		std::map<GameObject, std::array<GLfloat, 3>> sceneData;
		std::array<GLfloat, 3> startingSplitter;

	public:

		// Constructors

		BSPTree();
		BSPTree(std::map<GameObject, std::array<GLfloat, 3>>);
		BSPTree(std::map<GameObject, GLfloat[3]>, std::array<GLfloat, 3>);
		
		// Functions

		void setSceneData(std::map<GameObject, std::array<GLfloat, 3>>);
		void setStartingSplitter(std::array<GLfloat, 3>);
		void createTree();
		void addGameObject(GameObject o, std::array<GLfloat, 3>);
		void traverse();
		void traverse(BSPTreeNode n);

};

