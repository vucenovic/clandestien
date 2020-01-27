#pragma once
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include "Material.h"
#include <vector>

//overall this whole thing needs a lot of refactoring, but coming up with a good structure ist difficult
namespace Particles
{
	struct Particle { //maybe switch particles to a non interleaved format to allow only writing whats needed?
		glm::vec3 postion;
		glm::vec3 velocity;
		glm::vec4 color;
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

		const GLuint GetBufferHandle() const;

		void Bind() const;
	};

	template<typename T>
	class AnimatedValueKey {
	public:
		T value;
		float time;
	public:
		const T & GetValue() const { return value; };

		AnimatedValueKey() : time(0), value(T()) {};
		AnimatedValueKey(float time, T value) : time(time), value(value) {};

		bool operator > (const AnimatedValueKey& o) { return time > o.time; };
		bool operator < (const AnimatedValueKey& o) { return time < o.time; };
	};

	template<typename T>
	class AnimatedValue {
	private:
		std::vector<AnimatedValueKey<T>> keys;
		bool loop;
		float currentTime;
	public:
		inline const T Evaluate(const float time) const {
			return keys[0].GetValue();
		}
	};

	class ParticleSystemDefinition {
	public:
		size_t maxParticles;
		float lifeTime;
		glm::vec3 constantForce = glm::vec3(0);
		std::shared_ptr<Material> material;

		AnimatedValue<float> size;
		AnimatedValue<float> rotation;
		AnimatedValue<glm::vec4> color;
	};

	class ParticleSystem
	{
	private:
		ParticleSystemDefinition & prototype;
		std::shared_ptr<ParticleSystemMeshManager> meshManager;

		size_t indexStart, count;
		size_t activeParticles;

		Particle * particles;
		float * particleLifetimes;
		
		void RemoveDeadParticles();
		void SpawnNewParticles();
	public:
		ParticleSystem(ParticleSystemDefinition & prototype);
		~ParticleSystem();

		void CreateDebugParticles(); //temporary function to test with until the remaining systems are in place
		void AddDebugParticles();

		void Update(const float deltaTime);

		void WriteMesh(const Particle* PBuffer, const size_t minIndex, const size_t maxIndex);

		void Draw() const; //meshManager must be Bound before calling draw on the ParticleSystem

		static void PrepareDraw(); //TODO move this stuff into a proper renderer
		static void FinishDraw();
	};
}