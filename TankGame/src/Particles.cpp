#include "Particles.h"
#include <random>

namespace Particles {

	ParticleSystemMeshManager::ParticleSystemMeshManager(size_t maxParticles)
	{
		maxParticleCount = maxParticles;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &ParticleBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * maxParticleCount, nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const void*)(4 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	ParticleSystemMeshManager::~ParticleSystemMeshManager()
	{
		if (buffer != nullptr) End();
		glDeleteBuffers(1, &ParticleBuffer);
		glDeleteVertexArrays(1, &VAO);
	}

	void ParticleSystemMeshManager::Begin()
	{
		glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffer);
		buffer = (Particle *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	}

	Particle * ParticleSystemMeshManager::GetBuffer()
	{
		return buffer;
	}

	void ParticleSystemMeshManager::End()
	{
		buffer = nullptr;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void ParticleSystemMeshManager::Bind() const
	{
		glBindVertexArray(VAO);
	}

	ParticleSystem::ParticleSystem(const ParticleSystemPrototype & prototype)
	{
		const size_t pcount = 32;
		std::default_random_engine gen;
		std::uniform_real_distribution<float> distr1(-0.25f, 0.25f);
		std::uniform_real_distribution<float> distr2(0.5f, 1.5f);

		GLfloat * data = new GLfloat[PARTICLE_FLOAT_SIZE * pcount];
		for (int i = 0; i < pcount; i++) {
			data[i*PARTICLE_FLOAT_SIZE] = distr1(gen);
			data[i*PARTICLE_FLOAT_SIZE + 1] = distr1(gen);
			data[i*PARTICLE_FLOAT_SIZE + 2] = distr1(gen);
			data[i*PARTICLE_FLOAT_SIZE + 3] = distr2(gen);
		}
	}

	void ParticleSystem::WriteMesh(const void * PBuffer, const size_t minIndex, const size_t maxIndex)
	{
		count = maxIndex - minIndex;
		indexStart = minIndex;
		memcpy((Particle*)PBuffer + minIndex, particles, count * sizeof(Particle));
	}

	void ParticleSystem::Draw() const
	{
		const Material & mat = *(prototype.material);
		mat.Use(); //TODO move this stuff into a proper render manager

		GLuint modelMatrixLocation = mat.shader->GetUniformLocation("modelMatrix");
		GLuint normalMatrixLocation = mat.shader->GetUniformLocation("modelNormalMatrix");
		glm::mat4 ident = glm::mat4(1);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(ident));

		glDrawArrays(GL_POINTS, indexStart, count);//except this, this should then be inlined
	}

	void ParticleSystem::PrepareDraw()
	{ //TODO move into a proper render manager
		glEnable(GL_BLEND);
		glDepthMask(false);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
	}

	void ParticleSystem::FinishDraw()
	{
		glDisable(GL_BLEND);
		glDepthMask(true);
	}

	inline const float AnimatedValue::Evaluate(const float time) const
	{
		return keys.value;
	}
}