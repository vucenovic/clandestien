#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>
#include <memory>
#include "Utils.h"

class Mesh {
private:
	GLuint vaoID;
	size_t indicesCount;
	BoundingBox bounds;
public:
	Mesh();
	~Mesh();

	const size_t GetIndicesCount() const;

	void Bind();
	void BindAndDraw();
	void Draw();

	static std::unique_ptr<Mesh> InterleavedPNT(unsigned int vertexCount, const GLfloat data[], unsigned int faceCount, const GLushort indices[]);//pos,norm,tex
	static std::unique_ptr<Mesh> InterleavedPNTT(unsigned int vertexCount, const GLfloat data[], unsigned int faceCount, const GLushort indices[]);//pos,norm,tex,tangent
	static std::unique_ptr<Mesh> SimpleIndexed(unsigned int vertexCount, const GLfloat vertices[], unsigned int faceCount, const GLushort indices[]);
	static std::unique_ptr<Mesh> SimpleIndexed(unsigned int vertexCount, const GLfloat vertices[], const GLfloat normals[], unsigned int faceCount, const GLushort indices[]);
	static std::unique_ptr<Mesh> SimpleIndexed(unsigned int vertexCount, const GLfloat vertices[], const GLfloat normals[], const GLfloat textureCoords[], unsigned int faceCount, const GLushort indices[]);
	static std::unique_ptr<Mesh> SimpleIndexed(unsigned int vertexCount, const GLfloat vertices[], const GLfloat normals[], const GLfloat textureCoords[], const GLfloat tangents[], unsigned int faceCount, const GLushort indices[]);
};

class OBJLoader {
public:
	static std::unique_ptr<Mesh> LoadOBJ(const std::string & filePath);
};

class MeshBuilder {
public:
	static std::unique_ptr<Mesh> Quad();
	static std::unique_ptr<Mesh> Quad(GLfloat minx, GLfloat miny, GLfloat maxx, GLfloat maxy);
	static std::unique_ptr<Mesh> BoxFlatShaded(float x, float y, float z);
	static std::unique_ptr<Mesh> CylinderSplitShaded(float height, float radius, unsigned int segmentCount);
	static std::unique_ptr<Mesh> Sphere(float radius, unsigned int segmentsHorizontal, unsigned int segmentsVertical);
	static std::unique_ptr<Mesh> Torus(float radiusMajor, float radiusMinor, unsigned int segmentsMajor, unsigned int segmentsMinor);
};