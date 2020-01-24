#pragma once
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include "Material.h"

namespace Particles
{
	struct Particle {
		glm::vec3 postion;
		GLfloat size;
		GLfloat rotation;
	};
	constexpr size_t PARTICLE_FLOAT_SIZE = sizeof(Particle) / sizeof(GLfloat);

	class ParticleSystemMeshManager{
	private:
		size_t maxParticleCount;
		GLuint VAO;
		GLuint ParticleBuffer;
	public:
		ParticleSystemMeshManager(size_t maxParticles);
		~ParticleSystemMeshManager();

		void Bind() const;
	};

	class AnimatedValueKey {
	public:
		float value;
		float time;
	public:

	};

	class AnimatedValue {
	private:
		AnimatedValueKey keys;
	public:
		inline const float Evaluate(const float time) const;
	};

	class ParticleSystemPrototype {
	public:
		size_t maxParticles;
		std::shared_ptr<Material> material;
	};

	class ParticleSystem
	{
	private:
		ParticleSystemPrototype prototype;
		std::shared_ptr<ParticleSystemMeshManager> meshManager;

		size_t indexStart, count;
		size_t activeParticles;

		Particle * particles;
	public:
		ParticleSystem(const ParticleSystemPrototype & prototype);
		~ParticleSystem();

		void WriteMesh(const void* PBuffer, const size_t minIndex, const size_t maxIndex);

		void Draw() const; //meshManager must be Bound before calling draw on the ParticleSystem

		static void PrepareDraw(); //TODO move this stuff into a proper renderer
		static void FinishDraw();
	};
}