#include "BSPTree.h"

BSPTree::BSPTree()
{
}

BSPTree::BSPTree(std::map<GameObject, std::array<GLfloat, 3>> sceneData)
{
	this->sceneData = sceneData;
}

void BSPTree::setSceneData(std::map<GameObject, std::array<GLfloat, 3>> sceneData)
{
	this->sceneData = sceneData;
}

BSPTree::BSPTree(std::map<GameObject, GLfloat[3]> sceneData, std::array<GLfloat, 3> centroid)
{
	this->startingSplitter = centroid;
}

void BSPTree::setStartingSplitter(std::array<GLfloat, 3> centroid) 
{
	this->startingSplitter = centroid;
}

void BSPTree::createTree()
{
	
}

void BSPTree::addGameObject(GameObject o, std::array<GLfloat, 3> centroid)
{
	//this->sceneData.emplace(o, centroid);
}

void BSPTree::traverse()
{
	BSPTreeNode current = this->root;

}

void BSPTree::traverse(BSPTreeNode n)
{
}
