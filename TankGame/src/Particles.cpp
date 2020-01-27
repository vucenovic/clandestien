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

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0); //position
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const void*)(3 * sizeof(GLfloat))); //velocity
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (const void*)(6 * sizeof(GLfloat))); //color
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const void*)(10 * sizeof(GLfloat))); //size
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const void*)(11 * sizeof(GLfloat))); //rotation
		glEnableVertexAttribArray(4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	ParticleSystemMeshManager::~ParticleSystemMeshManager()
	{
		glDeleteBuffers(1, &ParticleBuffer);
		glDeleteVertexArrays(1, &VAO);
	}

	const GLuint ParticleSystemMeshManager::GetBufferHandle() const
	{
		return ParticleBuffer;
	}

	void ParticleSystemMeshManager::Bind() const
	{
		glBindVertexArray(VAO);
	}

	void ParticleSystem::RemoveDeadParticles()
	{
		size_t end = 0;
		size_t emptyCount = 0;
		size_t start = 0;
		bool rem = false;

		for (size_t i = 0; i < activeParticles; i++) {
			if (particleLifetimes[i] > prototype.lifeTime) {
				if (rem) {
					if (end != 0) {
						memmove(particleLifetimes + end, particleLifetimes + start, (i - start) * sizeof(float));
						memmove(particles + end, particles + start, (i - start) * sizeof(float));
					}
					emptyCount = 0;
					rem = false;
					end += i - start;
				}
				emptyCount++;
			}
			else {
				if (!rem) start = i;
				rem = true;
			}
		}
		if (rem) {
			if (end != 0) {
				memmove(particleLifetimes + end, particleLifetimes + start, (activeParticles - start) * sizeof(float));
				memmove(particles + end, particles + start, (activeParticles - start) * sizeof(float));
				end += activeParticles - start;
				activeParticles = end;
			}
		}
		else activeParticles = 0;
	}

	void ParticleSystem::SpawnNewParticles()
	{
	}

	ParticleSystem::ParticleSystem(ParticleSystemDefinition & prototype) : prototype(prototype)
	{
		particles = new Particle[prototype.maxParticles];
		particleLifetimes = new float[prototype.maxParticles];
	}

	ParticleSystem::~ParticleSystem()
	{
		delete[] particles;
		delete[] particleLifetimes;
	}

	void ParticleSystem::CreateDebugParticles()
	{
		std::default_random_engine gen;
		std::normal_distribution<float> distr1(0, 1);
		std::uniform_real_distribution<float> distr2(0.5f, 1.5f);

		for (int i = 0; i < prototype.maxParticles; i++) {
			Particle & p = particles[i];
			p.postion = glm::vec3(distr1(gen), distr1(gen), distr1(gen));
			p.velocity = glm::vec3(distr1(gen), distr1(gen), distr1(gen)) * 0.1f;
			p.rotation = distr1(gen);
			p.size = distr2(gen);
			particleLifetimes[i] = 0;
		}
		activeParticles = prototype.maxParticles;
	}

	void ParticleSystem::Update(const float deltaTime)
	{
		RemoveDeadParticles();
		for (size_t i = 0; i < activeParticles; i++) {
			particleLifetimes[i] += deltaTime; //increase lifetimes
			particles[i].velocity += prototype.constantForce * deltaTime; //integrate velocity
			particles[i].postion += particles[i].velocity * deltaTime; //integrate postion
		}
	}

	void ParticleSystem::WriteMesh(const Particle * PBuffer, const size_t offset, const size_t maxCount)
	{
		count = maxCount < activeParticles ? maxCount : activeParticles;
		indexStart = offset;
		memcpy((void*)(PBuffer + offset), particles, count * sizeof(Particle));
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

	//Sets the render state for drawing particle systems
	void ParticleSystem::PrepareDraw()
	{ //TODO move into a proper render manager
		glEnable(GL_BLEND);
		glDepthMask(false);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
	}

	//sets the render state for rendering regular geometry (this should be moved somewhere else really)
	void ParticleSystem::FinishDraw()
	{
		glDisable(GL_BLEND);
		glDepthMask(true);
	}
}