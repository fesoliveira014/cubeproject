#ifndef _CHUNK_MANAGER_H_
#define _CHUNK_MANAGER_H_

#include "../Common.h"

#include "Chunk.h"
#include  "mesher\ChunkMesher.hpp"
#include "../math/noise/noise.h"
#include "../math/fastNoise/FastNoise.h"
#include "../math/Picking.h"

#include  "../render/Renderer.h"

#include "../utils/ThreadPool.h"

#include <vector>

namespace tactical
{
	using namespace volume;

	class render::Shader;

	using ChunkMap = std::unordered_map<glm::vec3, std::shared_ptr<Chunk>,
		std::hash<glm::vec3>, std::equal_to<glm::vec3>>;
	using ChunkIterator = std::unordered_map<glm::vec3, std::shared_ptr<Chunk>,
		std::hash<glm::vec3>, std::equal_to<glm::vec3>>::iterator;

	class ChunkManager
	{
	public:
		ChunkManager(render::Renderer* pRenderer, const glm::vec3& worldDimension, int seed = 0);
		~ChunkManager();

		void FillChunks();
		void FillWithPyramids();
		void GenerateWorld();

		void UpdateChunkMesh(Chunk& chunk);

		void UpdateChunks(const glm::vec3& currentPos);

		void Draw(std::string shaderID);

		void ThreadDrawTask(std::string shaderID, ChunkIterator begin, ChunkIterator end);
		void ThreadUpdateTask(const glm::vec3& currentPos, ChunkIterator begin, ChunkIterator end);

		inline void SetChunkSize(int size) { m_chunkSize = size; }
		inline int GetChunkSize() const { return m_chunkSize; }

		inline std::shared_ptr<Chunk> GetChunk(const glm::vec3& pos)
		{
			if (m_chunks[pos] != nullptr)
				return m_chunks[pos];
			return nullptr;
		}

		void SetCurrentChunk(const glm::vec3& pos);
		inline glm::vec3 GetCurrentChunk() { return m_currentChunk; }

		inline void SetChunkLoadingRadius(int radius) { m_chunkLoadingRadius = radius; }
		inline int GetChunkLoadingRadius() const { return m_chunkLoadingRadius; }

		math::RayCastResult GetRayVoxelIntersection(math::Ray& ray, const glm::vec3& pos, float pickRadius);

		inline ChunkIterator begin() { return m_chunks.begin(); }
		inline ChunkIterator end() { return m_chunks.end(); }

		glm::vec3 ChunkManager::World2Chunk(const glm::vec3& pos);
		glm::vec3 ChunkManager::World2Voxel(const glm::vec3& pos);
		byte ChunkManager::GetVoxel(const glm::vec3& pos);

		void SetVoxel(const glm::vec3& pos, byte type);
		void SetVoxel(float x, float y, float z, byte type) { SetVoxel(glm::vec3(x, y, z), 1); }

	private:
		enum ChunkUpdateType { Mesh, Render, None };

		ChunkManager();
		void Initialize();

		bool IsWithinRadius(const glm::vec3& position);
		void Draw(std::shared_ptr<Chunk> chunk, render::Shader& shader);
		std::vector<int> SplitMemToBounds(int mem, int parts);

		glm::vec3 GridCoordsToWorldCoords(const glm::vec3& pos);

		void GeneratePyramid(Chunk &chunk);

		ChunkMap m_chunks;
		std::queue<std::shared_ptr<Chunk> > m_chunksToUpdate;
		
		std::atomic<ChunkUpdateType> m_chunkUpdateType;

		int m_chunkSize;
		int m_maxWorldHeight;
		int m_chunkLoadingRadius; // in number of chunks, not in coordinates
		glm::vec3 m_currentChunk;
		glm::vec3 m_worldDimensions;
		std::vector<int> m_bounds;

		render::Renderer* m_pRenderer;

		FastNoise m_fastnoise;

		utils::ThreadPool m_threadPool;

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