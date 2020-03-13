#include "BSPTree.h"

BSPTree::BSPTree()
{
	this->root = nullptr;
}

BSPTree::BSPTree(std::map<GameObject*, std::array<GLfloat, 3>> sceneData)
{
	this->sceneData = sceneData;
	this->root = nullptr;
}

void BSPTree::setSceneData(std::map<GameObject*, std::array<GLfloat, 3>> sceneData)
{
	this->sceneData = sceneData;
	this->root = nullptr;
}

BSPTree::BSPTree(std::map<GameObject*, std::array<GLfloat, 3>> sceneData, std::array<GLfloat, 3> centroid)
{
	this->sceneData = sceneData;
	this->root = nullptr;
	this->startingSplitter = centroid;
}

void BSPTree::setStartingSplitter(std::array<GLfloat, 3> centroid) 
{
	this->startingSplitter = centroid;
}

void BSPTree::createTree()
{
	
}

void BSPTree::addGameObject(GameObject *o, std::array<GLfloat, 3> centroid)
{
	this->sceneData.insert({o, centroid }); 
}

void BSPTree::traverseInOrder()
{
	BSPTreeNode *current = this->root;
	if (current != nullptr) {
		this->traverseInOrder(root->left);
		// TODO: do something with value
		this->traverseInOrder(root->right);
	}
}

void BSPTree::traverseInOrder(BSPTreeNode *n)
{
	if (n != nullptr) {
		this->traverseInOrder(n->left);
		// TODO: do something with value
		this->traverseInOrder(n->right);
	}
}
