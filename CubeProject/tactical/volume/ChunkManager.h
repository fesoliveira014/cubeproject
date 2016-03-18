#ifndef _CHUNK_MANAGER_H_
#define _CHUNK_MANAGER_H_

#include "../Common.h"

#include "Chunk.h"
#include "../math/noise/noise.h"

#include  "../render/Renderer.h"

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
		ChunkManager(render::Renderer* pRenderer, const glm::ivec3& worldDimension, int seed = 0);
		~ChunkManager();

		void FillChunks();
		void GenerateWorld();

		void UpdateChunks(const glm::ivec3& currentPos);

		void Draw(std::string shaderID);

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

		glm::vec3 GetRayVoxelIntersection(math::Ray& ray, const glm::vec3& pos, float pickRadius);

		inline ChunkIterator begin() { return m_chunks.begin(); }
		inline ChunkIterator end() { return m_chunks.end(); }

	private:
		ChunkManager();
		void Initialize();
		
		bool IsWithinRadius(const glm::ivec3& position);
		void Draw(Chunk* chunk, render::Shader& shader);

		// Returns coordinates of the chunk a point is in, if any
		glm::ivec3 WorldCoordsToGridCoords(const glm::ivec3& pos);

		glm::ivec3 GridCoordsToWorldCoords(const glm::ivec3& pos);

		ChunkMap m_chunks;

		bool m_meshNeedsUpdate;
	
		int m_chunkSize;
		int m_maxWorldHeight;
		int m_chunkLoadingRadius; // in number of chunks, not in coordinates
		glm::ivec3 m_currentChunk;
		glm::ivec3 m_worldDimensions;

		render::Renderer* m_pRenderer;

		// these will be moved to a terrain engine in the future. 
		noise::module::RidgedMulti m_mountains;
		noise::module::Billow m_baseFlat;
		noise::module::ScaleBias m_flat;
		noise::module::Perlin m_perlin;
		noise::module::Select m_selector;
		noise::module::Turbulence m_final;
		noise::utils::NoiseMap m_heightMap;

	};
}

#endif