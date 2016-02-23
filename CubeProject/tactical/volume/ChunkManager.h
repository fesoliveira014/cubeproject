#ifndef _CHUNK_MANAGER_H_
#define _CHUNK_MANAGER_H_

#include "../Common.h"
#include "../math/PerlinNoise.h"

#include "Chunk.h"
#include "../math/PerlinNoise.h"

namespace tactical
{
	using namespace volume;

	class render::Shader;

	using ChunkMap = std::unordered_map<glm::vec3, Chunk,
			std::hash<glm::vec3>, std::equal_to<glm::vec3>> ;
	using ChunkIterator = std::unordered_map<glm::vec3, Chunk,
			std::hash<glm::vec3>, std::equal_to<glm::vec3>>::iterator;

	class ChunkManager
	{
	public:
		ChunkManager();
		ChunkManager(int chunkSize);
		~ChunkManager();

		void FillChunks();
		void GenerateWorld();

		void Draw(const glm::vec3& center, render::Shader& shader);

		inline void SetChunkSize(int size) { m_chunkSize = size; }
		inline int GetChunkSize() const { return m_chunkSize; }

		inline void SetNoise(math::PerlinNoise2D& noise) { m_noise = noise; }
		inline void SetNoise(double persistance, double frequency, double amplitude, int octaves, int seed) 
		{ 
			m_noise.Set(persistance, frequency, amplitude, octaves, seed); 
		}

		inline math::PerlinNoise2D* GetNoise() { return &m_noise; }

		inline ChunkIterator begin() { return m_chunks.begin(); }
		inline ChunkIterator end() { return m_chunks.end(); }
	private:
		ChunkMap m_chunks;

		int m_numChunks;
		int m_chunkSize;
		glm::vec3 m_currentChunk;

		math::PerlinNoise2D m_noise;
	};
}

#endif