#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>
#include <memory>

class Mesh {
private:
	GLuint vaoID;
	size_t indicesCount;
public:
	Mesh();
	~Mesh();

	const size_t GetIndicesCount() const;

	void Bind();
	void BindAndDraw();
	void Draw();

	static std::unique_ptr<Mesh> InterleavedPNT(unsigned int vertexCount, const GLfloat data[], unsigned int faceCount, const GLushort indices[]);
	static std::unique_ptr<Mesh> SimpleIndexed(unsigned int vertexCount, const GLfloat vertices[], unsigned int faceCount, const GLushort indices[]);
	static std::unique_ptr<Mesh> SimpleIndexed(unsigned int vertexCount, const GLfloat vertices[], const GLfloat normals[], unsigned int faceCount, const GLushort indices[]);
	static std::unique_ptr<Mesh> SimpleIndexed(unsigned int vertexCount, const GLfloat vertices[], const GLfloat normals[], const GLfloat textureCoords[], unsigned int faceCount, const GLushort indices[]);
};

class OBJLoader {
public:
	static std::unique_ptr<Mesh> LoadOBJ(const std::string & filePath);
};

class MeshBuilder {
public:
	static std::unique_ptr<Mesh> Quad();
	static std::unique_ptr<Mesh> BoxFlatShaded(float x, float y, float z);
	static std::unique_ptr<Mesh> CylinderSplitShaded(float height, float radius, unsigned int segmentCount);
	static std::unique_ptr<Mesh> Sphere(float radius, unsigned int segmentsHorizontal, unsigned int segmentsVertical);
	static std::unique_ptr<Mesh> Torus(float radiusMajor, float radiusMinor, unsigned int segmentsMajor, unsigned int segmentsMinor);
};