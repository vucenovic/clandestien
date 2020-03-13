#pragma once

#include "BSPTreeNode.h"
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

		//Variables

		BSPTreeNode root;
		BSPTreeNode rightChild;
		BSPTreeNode leftChild;
		std::map<GameObject, GLfloat[3]> sceneData;

	public:

		// Constructors

		BSPTree();
		BSPTree(std::map<GameObject,GLfloat[3]>);		// calls createTree upon initialization
		
		// Functions

		void setSceneData(std::map<GameObject, std::array<GLfloat, 3>>);
		void createTree(GLfloat centroid[]);
		void addGameObject(GameObject o);
		void traverse();
		void traverse(BSPTreeNode n);

};

