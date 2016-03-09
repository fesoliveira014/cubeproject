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

	using ChunkMap = std::unordered_map<glm::ivec3, Chunk*,
			std::hash<glm::ivec3>, std::equal_to<glm::ivec3>> ;
	using ChunkIterator = std::unordered_map<glm::ivec3, Chunk*,
			std::hash<glm::ivec3>, std::equal_to<glm::ivec3>>::iterator;

	class ChunkManager
	{
	public:
		ChunkManager(const glm::ivec3& worldDimension, int seed = 0);
		ChunkManager(const glm::ivec3& worldDimension, const glm::ivec3& initalPosition, int seed = 0);
		~ChunkManager();

		void FillChunks();
		void GenerateWorld();

		void UpdateChunks(const glm::ivec3& currentPos);

		void Draw(render::Shader& shader);

		inline void SetChunkSize(int size) { m_chunkSize = size; }
		inline int GetChunkSize() const { return m_chunkSize; }

		inline Chunk* GetChunk(const glm::ivec3& pos) 
		{
			if (m_chunks[pos] != nullptr)
				return m_chunks[pos];
			return nullptr;
		}

		void SetCurrentChunk(const glm::ivec3& pos);
		inline glm::ivec3 GetCurrentChunk() { return m_currentChunk; }

		inline void SetChunkLoadingRadius(int radius) { m_chunkLoadingRadius = radius; }
		inline int GetChunkLoadingRadius() const { return m_chunkLoadingRadius; }

		inline void SetNoise(math::PerlinNoise2D& noise) { m_noise = noise; }
		inline void SetNoise(double persistance, double frequency, double amplitude, int octaves, int seed);

		inline math::PerlinNoise2D* GetNoise() { return &m_noise; }

		inline ChunkIterator begin() { return m_chunks.begin(); }
		inline ChunkIterator end() { return m_chunks.end(); }

	private:
		ChunkManager();
		void Initialize();

		bool CreateChunk(const glm::ivec3& pos);
		void RecursiveChunkUpdate(Chunk* chunk);
		bool IsWithinRadius(const glm::ivec3& position);
		void Draw(Chunk* chunk, render::Shader& shader);

		ChunkMap m_chunks;
	
		int m_chunkSize;
		int m_maxWorldHeight;
		int m_chunkLoadingRadius; // in number of chunks, not in coordinates
		glm::ivec3 m_currentChunk;
		glm::ivec3 m_worldDimensions;

		math::PerlinNoise2D m_noise;

	};
}

#endif