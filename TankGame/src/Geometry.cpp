#include "Geometry.h"
#include <glm\gtc\matrix_transform.hpp>
#include <fstream>
#include <string>
#include <unordered_map>

Mesh::Mesh()
{
	indicesCount = 0;
	glGenVertexArrays(1, &vaoID);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vaoID);
}

void Mesh::Bind()
{
	glBindVertexArray(vaoID);
}

const size_t Mesh::GetIndicesCount() const
{
	return indicesCount;
}

void Mesh::BindAndDraw()
{
	Bind();
	Draw();
}

void Mesh::Draw()
{
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, 0);
}

std::unique_ptr<Mesh> Mesh::InterleavedPNT(unsigned int vertexCount, const GLfloat data[], unsigned int faceCount, const GLushort indices[])
{
	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

	mesh->Bind();
	GLuint buffers[2];
	glGenBuffers(2, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 8 * sizeof(GLfloat), data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * sizeof(GLushort), indices, GL_STATIC_DRAW);
	mesh->indicesCount = faceCount * 3;

	glBindVertexArray(0);
	glDeleteBuffers(2, buffers);

	return mesh;
}

std::unique_ptr<Mesh> Mesh::SimpleIndexed(unsigned int vertexCount, const GLfloat vertices[], unsigned int faceCount, const GLushort indices[])
{
	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

	mesh->Bind();
	GLuint vbos[2];
	glGenBuffers(2, vbos);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);

	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * sizeof(GLushort), indices, GL_STATIC_DRAW);
	mesh->indicesCount = faceCount * 3;

	glBindVertexArray(0);
	glDeleteBuffers(2, vbos);

	return mesh;
}

std::unique_ptr<Mesh> Mesh::SimpleIndexed(unsigned int vertexCount, const GLfloat vertices[], const GLfloat normals[], unsigned int faceCount, const GLushort indices[])
{
	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

	mesh->Bind();
	GLuint vbos[3];
	glGenBuffers(3, vbos);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);

	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);

	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * sizeof(GLushort), indices, GL_STATIC_DRAW);
	mesh->indicesCount = faceCount * 3;

	glBindVertexArray(0);
	glDeleteBuffers(3, vbos);

	return mesh;
}

std::unique_ptr<Mesh> Mesh::SimpleIndexed(unsigned int vertexCount, const GLfloat vertices[], const GLfloat normals[], const GLfloat textureCoords[], unsigned int faceCount, const GLushort indices[])
{
	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

	mesh->Bind();
	GLuint vbos[4];
	glGenBuffers(4, vbos);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);

	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);

	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);

	glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(GLfloat), textureCoords, GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[3]);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * sizeof(GLushort), indices, GL_STATIC_DRAW);
	mesh->indicesCount = faceCount * 3;

	glBindVertexArray(0);
	glDeleteBuffers(4, vbos);

	return mesh;
}

std::unique_ptr<Mesh> MeshBuilder::Quad()
{
	static const GLfloat verts[4*3] = {1,1,0,1,-1,0,-1,-1,0,-1,1,0};
	static const GLfloat norms[4 * 3] = { 0,0,1,0,0,1,0,0,1,0,0,1 };
	static const GLfloat texcoords[4 * 2] = { 1,1,1,0,0,0,0,1 };
	static const GLushort inds[2*3] = {2,1,0,0,3,2};
	return Mesh::SimpleIndexed(4, verts, norms, texcoords, 2, inds);
}

std::unique_ptr<Mesh> MeshBuilder::BoxFlatShaded(float x, float y, float z)
{
	float xh = x / 2, yh = y / 2, zh = z / 2;
	const GLfloat verts[4 * 6 * 3] = {
		xh,yh,zh, xh,-yh,zh, -xh,-yh,zh, -xh,yh,zh, //+z
		xh,yh,-zh, xh,-yh,-zh, -xh,-yh,-zh, -xh,yh,-zh, //-z
		xh,yh,zh, -xh,yh,zh, xh,yh,-zh, -xh,yh,-zh, //+y
		xh,-yh,zh, -xh,-yh,zh, xh,-yh,-zh, -xh,-yh,-zh, //-y
		xh,yh,zh, xh,yh,-zh, xh,-yh,zh, xh,-yh,-zh, //+x
		-xh,yh,zh, -xh,yh,-zh, -xh,-yh,zh, -xh,-yh,-zh, //-x
	};
	static const GLfloat norms[4 * 6 * 3] = {
		0,0,1,0,0,1,0,0,1,0,0,1,
		0,0,-1,0,0,-1,0,0,-1,0,0,-1,
		0,1,0,0,1,0,0,1,0,0,1,0,
		0,-1,0,0,-1,0,0,-1,0,0,-1,0,
		1,0,0,1,0,0,1,0,0,1,0,0,
		-1,0,0,-1,0,0,-1,0,0,-1,0,0,
	};
	static const GLfloat texcoords[4 * 6 * 2] = {
		1,1,1,0,0,0,0,1,
		0,1,0,0,1,0,1,1,
		1,0,0,0,1,1,0,1,
		1,1,0,1,1,0,0,0,
		0,1,1,1,0,0,1,0,
		1,1,0,1,1,0,0,0,
	};
	static const GLushort inds[12 * 3] = {
		2,1,0,3,2,0, //+z
		4,5,6,4,6,7, //-z
		8,10,9,9,10,11, //+y;
		12,13,14,13,15,14, //-y
		16,18,17,17,18,19, //+x
		20,21,22,21,23,22//-x
	};
	return Mesh::SimpleIndexed(4 * 6, verts, norms, texcoords, 12, inds);
}

std::unique_ptr<Mesh> MeshBuilder::CylinderSplitShaded(float h, float r, unsigned int n)
{
	if (n < 3) return std::make_unique<Mesh>(); //doesn't make sense under 3 segments
	float hh = h / 2;
	size_t vertexCount = (4 * n + 4);
	GLfloat* verts = new GLfloat[vertexCount * 3];
	GLushort* inds = new GLushort[(4 * n) * 3];
	GLfloat* normals = new GLfloat[vertexCount * 3];
	GLfloat* texcoords = new GLfloat[vertexCount * 2];

	//Generate Vertices and Normals
	size_t ai = 0;
	size_t ai2 = 0;
	//centers
	normals[ai] = 0; verts[ai++] = 0;
	normals[ai] = 1; verts[ai++] = hh;
	normals[ai] = 0; verts[ai++] = 0;
	texcoords[ai2++] = 0.5; texcoords[ai2++] = 0.5;

	normals[ai] = 0; verts[ai++] = 0;
	normals[ai] = -1; verts[ai++] = -hh;
	normals[ai] = 0; verts[ai++] = 0;
	texcoords[ai2++] = 0.5; texcoords[ai2++] = 0.5;
	//Sides
	float angleStep = glm::two_pi<float>() / (float)n;
	float uvStep = 1 / (float)n;
	float c, s;
	for (size_t i = 0; i < n; i++)
	{
		c = glm::cos(angleStep*i);
		s = glm::sin(angleStep*i);

		normals[ai] = 0; verts[ai++] = c * r;
		normals[ai] = 1; verts[ai++] = hh;
		normals[ai] = 0; verts[ai++] = s * r;
		texcoords[ai2++] = 0.5f + c * 0.5f; texcoords[ai2++] = 0.5f + s * 0.5f;

		normals[ai] = c; verts[ai++] = c * r;
		normals[ai] = 0; verts[ai++] = hh;
		normals[ai] = s; verts[ai++] = s * r;
		texcoords[ai2++] = i * uvStep; texcoords[ai2++] = 1;

		normals[ai] = c; verts[ai++] = c * r;
		normals[ai] = 0; verts[ai++] = -hh;
		normals[ai] = s; verts[ai++] = s * r;
		texcoords[ai2++] = i * uvStep; texcoords[ai2++] = 0;

		normals[ai] = 0; verts[ai++] = c * r;
		normals[ai] = -1; verts[ai++] = -hh;
		normals[ai] = 0; verts[ai++] = s * r;
		texcoords[ai2++] = 0.5f + c * 0.5f; texcoords[ai2++] = 0.5f + s * 0.5f;
	}
	//Add final overlap
	normals[ai] = 1; verts[ai++] = r;
	normals[ai] = 0; verts[ai++] = hh;
	normals[ai] = 0; verts[ai++] = 0;
	texcoords[ai2++] = 1; texcoords[ai2++] = 1;

	normals[ai] = 1; verts[ai++] = r;
	normals[ai] = 0; verts[ai++] = -hh;
	normals[ai] = 0; verts[ai++] = 0;
	texcoords[ai2++] = 1; texcoords[ai2++] = 0;

	ai = 0;
	//Generate side Indices
	for (size_t i = 0; i < n - 1; i++)
	{
		GLushort vi = (GLushort)i * 4 + 2;//avoid recomputing the same thing 12 times

		inds[ai++] = vi + 4;
		inds[ai++] = vi;
		inds[ai++] = 0;

		inds[ai++] = vi + 5;
		inds[ai++] = vi + 2;
		inds[ai++] = vi + 1;

		inds[ai++] = vi + 5;
		inds[ai++] = vi + 6;
		inds[ai++] = vi + 2;

		inds[ai++] = 1;
		inds[ai++] = vi + 3;
		inds[ai++] = vi + 7;
	}
	//generate Last segment
	{
		GLushort n2 = (n-1) * 4 + 2;//avoid recomputation of the same thing
		inds[ai++] = 2;
		inds[ai++] = n2;
		inds[ai++] = 0;

		inds[ai++] = n2 + 4;
		inds[ai++] = n2 + 2;
		inds[ai++] = n2 + 1;

		inds[ai++] = n2 + 4;
		inds[ai++] = n2 + 5;
		inds[ai++] = n2 + 2;

		inds[ai++] = 1;
		inds[ai++] = n2 + 3;
		inds[ai++] = 5;
	}

	std::unique_ptr<Mesh> mesh = Mesh::SimpleIndexed(vertexCount, verts, normals, texcoords, (4 * n), inds);
	delete[] verts;
	delete[] inds;
	delete[] normals;
	delete[] texcoords;
	return mesh;
}

std::unique_ptr<Mesh> MeshBuilder::Sphere(float r, unsigned int nh, unsigned int nv)
{
	if (nh < 3 || nv < 2) return std::make_unique<Mesh>();
	size_t vertCount = ((nh+1) * (nv+1));
	size_t faceCount = (2 * nh * nv);
	GLfloat* verts = new GLfloat[vertCount * 3];
	GLfloat* normals = new GLfloat[vertCount * 3];
	GLfloat* texcoords = new GLfloat[vertCount * 2];
	GLushort* inds = new GLushort[faceCount * 3];

	//Generate Vertices and Normals
	size_t ai = 0;
	size_t ai2 = 0;
	//Sides
	float angleStepH = glm::two_pi<float>() / (float)nh;
	float angleStepV = glm::pi<float>() / (float)nv;
	float UVStepH = 1 / (float)nh;
	float UVStepV = 1 / (float)nv;
	static const float pihalf = glm::half_pi<float>();
	float ch, sh, cv, sv;
	for (size_t i = 0; i <= nv; i++)
	{
		cv = glm::cos(angleStepV*i - pihalf);
		sv = glm::sin(angleStepV*i - pihalf);

		for (size_t j = 0; j <= nh; j++)
		{
			ch = glm::cos(angleStepH * j); //could be computed beforehand and cached in an array since it's repeated (O(n) instead of O(n*m)) (Yes I am aware that this is not a performance critical section)
			sh = glm::sin(angleStepH * j);

			normals[ai] = ch * cv;	verts[ai++] = ch * cv * r;
			normals[ai] = sv;		verts[ai++] = sv * r;
			normals[ai] = sh * cv;	verts[ai++] = sh * cv * r;
			texcoords[ai2++] = UVStepH * j; texcoords[ai2++] = UVStepV * i;
		}
	}

	//Generate Indices
	ai = 0;
	GLushort nh1 = (nh + 1);
	for (GLushort i = 1; i < nv-1; i++)
	{
		GLushort sliceOffset = nh1 * i;
		for (size_t j = 0; j < nh; j++)
		{
			GLushort qo = sliceOffset + (GLushort)j;

			inds[ai++] = qo;
			inds[ai++] = qo + nh1;
			inds[ai++] = qo + nh1 + 1;

			inds[ai++] = qo;
			inds[ai++] = qo + nh1 + 1;
			inds[ai++] = qo + 1;
		}
	}
	GLushort sliceOffset = nh1 * (nv-1);
	for (GLushort j = 0; j < nh; j++)
	{
		GLushort qo = sliceOffset + j;

		inds[ai++] = j;
		inds[ai++] = j + nh1;
		inds[ai++] = j + nh1 + 1;

		inds[ai++] = qo;
		inds[ai++] = qo + nh1 + 1;
		inds[ai++] = qo + 1;
	}

	//There are two vertices that remain unused (but I am unwilling to increase the complexity of this just to remove two vertices)

	std::unique_ptr<Mesh> mesh = Mesh::SimpleIndexed(vertCount, verts, normals, texcoords, faceCount, inds);
	delete[] verts;
	delete[] inds;
	delete[] normals;
	delete[] texcoords;
	return mesh;
}

std::unique_ptr<Mesh> MeshBuilder::Torus(float rl, float rt, unsigned int nl, unsigned int nt)
{
	if (nl < 3 || nt < 3) return std::make_unique<Mesh>();

	GLfloat* verts = new GLfloat[(nl*nt) * 3];
	GLushort* inds = new GLushort[(nl*nt*2) * 3];
	GLfloat* normals = new GLfloat[(nl*nt) * 3];

	//Generate Vertices
	float angleStepL = glm::two_pi<float>() / (float)nl;
	float angleStepT = glm::two_pi<float>() / (float)nt;
	float cl, sl, ct, st;
	size_t ai = 0;//array index counter for simplicity's sake
	for (size_t i = 0; i < nl; i++)
	{
		cl = glm::cos(angleStepL*i);
		sl = glm::sin(angleStepL*i);

		for (size_t j = 0; j < nt; j++)
		{
			ct = glm::cos(angleStepT * j); //could be computed beforehand and cached in an array since it's repeated (O(n) instead of O(n*m)) (Yes I am aware that this is not a performance critical section)
			st = glm::sin(angleStepT * j);

			normals[ai] = sl*ct; verts[ai++] = sl * rl + ct * sl*rt;
			normals[ai] = cl*ct; verts[ai++] = cl * rl + ct * cl*rt;
			normals[ai] = st; verts[ai++] = st*rt;
		}
	}

	//Generate side Indices
	ai = 0;//reuse counter
	for (GLushort i = 0; i < nl - 1; i++)
	{
		GLushort t = i * nt;

		for (GLushort j = 0; j < nt-1; j++)
		{
			GLushort tj = t + j;

			inds[ai++] = tj + nt + 1;
			inds[ai++] = tj + nt;
			inds[ai++] = tj;

			inds[ai++] = tj + 1;
			inds[ai++] = tj + nt + 1;
			inds[ai++] = tj;
		}
		inds[ai++] = t + nt;
		inds[ai++] = t + nt*2 - 1;
		inds[ai++] = t + nt - 1;

		inds[ai++] = t;
		inds[ai++] = t + nt;
		inds[ai++] = t + nt - 1;
	}
	//Generate Last Slice of faces
	GLushort t = nl * nt - nt;
	for (GLushort j = 0; j < nt-1; j++)
	{
		GLushort tj = t + j;

		inds[ai++] = j + 1;
		inds[ai++] = j;
		inds[ai++] = tj;

		inds[ai++] = tj + 1;
		inds[ai++] = j + 1;
		inds[ai++] = tj;
	}
	inds[ai++] = 0;
	inds[ai++] = nt - 1;
	inds[ai++] = t + nt - 1;

	inds[ai++] = t;
	inds[ai++] = 0;
	inds[ai++] = t + nt - 1;


	std::unique_ptr<Mesh> mesh = Mesh::SimpleIndexed(nl*nt, verts, normals, nl*nt * 2, inds);
	delete[] verts;
	delete[] inds;
	delete[] normals;
	return mesh;
}

std::unique_ptr<Mesh> OBJLoader::LoadOBJ(const std::string & filePath)
{
	GLushort vertexCount = 0;
	size_t indexCount = 0;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;

	struct vertDef {
		size_t pos, nor, tex;
		vertDef() : pos(0), nor(0), tex(0) {};

		inline bool operator==(const vertDef & r) const {
			return r.pos == pos && r.nor == nor && r.tex == tex;
		}
	};

	struct vertDefHasher {
		inline std::size_t operator()(const vertDef & k) const
		{
			return k.pos + (k.nor << 8) + (k.tex << 16);
		}
	};

	struct faceDef {
		vertDef v1, v2, v3;
	};

	struct lineReader { // TODO: generalize and keep as a faster (yet less safe) alternative to std::getline()
	public:
		char * lineBuffer;
		char * line;
		size_t bufferLength;
		size_t bufferOffset = 0;
		size_t bufferNext = 0;
		std::ifstream & file;

		size_t fileLength;
		bool last = true;

		//Bufferlength must be sufficient to contain the line TODO: make it resize when necessary
		lineReader(size_t bufferLength, std::ifstream & file, size_t fileLength) : bufferLength(bufferLength), lineBuffer(new char[bufferLength]), file(file),line(nullptr),fileLength(fileLength){
			file.read(lineBuffer, bufferLength);
		};
		~lineReader() { delete[] lineBuffer; };

		bool getline() {
			line = lineBuffer + bufferOffset;

			size_t nextBreak = bufferOffset;
			for (int i = bufferOffset; i < bufferLength; i++) {
				if (lineBuffer[i] == '\n') {
					nextBreak = i;
					break;
				}
			}
			if (nextBreak == bufferOffset) { //no endl found
				if (file.eof()) {
					bool ret = last;
					last = false;
					return ret;
				}
				else {
					size_t remLen = bufferLength - bufferOffset;
					memmove(lineBuffer, line, remLen);
					size_t before = file.tellg();
					file.read(lineBuffer+remLen, bufferOffset);
					if (file.eof()) {
						bufferLength = fileLength - before + remLen;
					}
					bufferOffset = 0;
					return getline(); //easiest way of doing this
				}
			}
			else {
				bufferOffset = nextBreak + 1;
				return true;
			}
		}
	};

	std::vector<faceDef> faces;

	std::ios::sync_with_stdio(false);
	std::ifstream file(filePath, std::ios::in | std::ios::ate);
	size_t fileLength = file.tellg();
	file.seekg(0, std::ios::beg);
	if (file.is_open()) {
		lineReader lr = lineReader(1024, file,fileLength);
		char * mid_ptr;
		while (lr.getline())
		{
			switch (lr.line[0])
			{
			case 'v': //vertex defintion
				switch (lr.line[1])
				{
				case 't': //texture
				{
					GLfloat u = std::strtof(lr.line + 2, &mid_ptr);
					GLfloat v = std::strtof(mid_ptr, nullptr);
					textureCoords.push_back(glm::vec2(u,v));
				}
					break;
				case 'n': //normal
				{
					GLfloat x = std::strtof(lr.line + 2, &mid_ptr);
					GLfloat y = std::strtof(mid_ptr, &mid_ptr);
					GLfloat z = std::strtof(mid_ptr, nullptr);
					normals.push_back(glm::vec3(x, y, z));
				}
					break;
				case ' ': //position
				{
					GLfloat x = std::strtof(lr.line + 2, &mid_ptr);
					GLfloat y = std::strtof(mid_ptr, &mid_ptr);
					GLfloat z = std::strtof(mid_ptr, nullptr);
					positions.push_back(glm::vec3(x, y, z));
					break;
				}
				default: //not supported
					break;
				}
				break;
			case 'f': //face defintion
			{
				faceDef face;
				face.v1.pos = std::strtoull(lr.line + 1, &mid_ptr, 10);
				face.v1.tex = std::strtoull(mid_ptr + 1, &mid_ptr, 10); //Needs to be written out like this due to the evaluation order
				face.v1.nor = std::strtoull(mid_ptr + 1, &mid_ptr, 10); //of function arguments being undefined
				face.v2.pos = std::strtoull(mid_ptr + 1, &mid_ptr, 10);
				face.v2.tex = std::strtoull(mid_ptr + 1, &mid_ptr, 10);
				face.v2.nor = std::strtoull(mid_ptr + 1, &mid_ptr, 10);
				face.v3.pos = std::strtoull(mid_ptr + 1, &mid_ptr, 10);
				face.v3.tex = std::strtoull(mid_ptr + 1, &mid_ptr, 10);
				face.v3.nor = std::strtoull(mid_ptr + 1, nullptr, 10);
				faces.push_back(face);
				break;
			}
			case '#': //Comment
			default: //not supported
				break;
			}
		}
		file.close();
	}

	struct VertexManager {
		struct Vertex {
			GLfloat px, py, pz, nx, ny, nz, tcu, tcv;
			Vertex(GLfloat px, GLfloat py, GLfloat pz, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat tcu, GLfloat tcv) : px(px), py(py), pz(pz), nx(nx), ny(ny), nz(nz), tcu(tcu), tcv(tcv) {};
		};

		std::unordered_map <vertDef, size_t, vertDefHasher> vertexDefs;
		std::vector<Vertex> vertexData;
		size_t runningCounter = 0;

		const std::vector<glm::vec3> & positions;
		const std::vector<glm::vec3> & normals;
		const std::vector<glm::vec2> & textureCoords;

		VertexManager(const std::vector<glm::vec3> & p, const std::vector<glm::vec3> & n, const std::vector<glm::vec2> & t) : positions(p), normals(n), textureCoords(t) { vertexData.reserve(p.size()); };

		//TODO: somehow calculate Tangents
		//leave space in the interleaved buffer and fill it later or add a second buffer object?
		size_t getIndex(vertDef v) {
			if (vertexDefs.find(v) == vertexDefs.end()) {
				glm::vec3 pos = positions[v.pos - 1];
				glm::vec3 nor = normals[v.nor - 1];
				glm::vec2 tex = textureCoords[v.tex - 1];
				vertexData.push_back(Vertex(pos.x,pos.y,pos.z,nor.x,nor.y,nor.z,tex.x,tex.y));
				vertexDefs[v] = runningCounter++; 
				return runningCounter - 1;
			}else{
				return vertexDefs[v];
			}
		}
	};

	std::vector<GLushort> indizes = std::vector<GLushort>();
	indizes.reserve(faces.size()*3);
	VertexManager VM = VertexManager(positions,normals,textureCoords);

	for (faceDef face : faces) {
		indizes.push_back(VM.getIndex(face.v1));
		indizes.push_back(VM.getIndex(face.v2));
		indizes.push_back(VM.getIndex(face.v3));
	}

	return Mesh::InterleavedPNT(VM.runningCounter, (GLfloat *)VM.vertexData.data(), faces.size(), indizes.data());
}