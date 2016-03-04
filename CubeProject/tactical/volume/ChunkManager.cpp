#include "ChunkManager.h"

namespace tactical
{
	using namespace volume;
	
	ChunkManager::ChunkManager()
		: m_chunkSize(16), m_numChunks(0), m_maxWorldHeight(64)
	{
		m_currentChunk = glm::ivec3(0);
		m_noise = math::PerlinNoise2D(0.5, 1.0 / 64.0, 4, 10, 0);
	}

	ChunkManager::ChunkManager(int seed)
		: m_chunkSize(16), m_numChunks(0), m_maxWorldHeight(64)
	{
		m_currentChunk = glm::ivec3(0);
		m_noise = math::PerlinNoise2D(0.5, 1.0 / 64.0, 4, 10, seed);
	}

	ChunkManager::ChunkManager(const glm::ivec3& initialPosition, int seed)
		: m_chunkSize(16), m_numChunks(0), m_maxWorldHeight(64), m_currentChunk(initialPosition)
	{
		m_noise = math::PerlinNoise2D(0.5, 1.0 / 64.0, 4, 10, seed);
	}

	ChunkManager::~ChunkManager()
	{
		for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
			delete iter->second;
		}
	}

	void ChunkManager::UpdateChunks(const glm::ivec3& currentPos)
	{
		glm::ivec3 offset(currentPos.x % m_chunkSize, currentPos.y % m_chunkSize, currentPos.z % m_chunkSize);
		glm::ivec3 chunkPos = currentPos - offset;

		if (currentPos != chunkPos) {
			SetCurrentChunk(currentPos);
			RecursiveChunkUpdate(m_chunks[currentPos]);
		}
	}

	void ChunkManager::RecursiveChunkUpdate(Chunk* chunk)
	{	
		glm::ivec3 vec = chunk->GetPosition() - m_currentChunk;
		int radius = m_chunkLoadingRadius * m_chunkSize;
		int radius2 = radius * radius;
		int distance2 = vec.x * vec.x + vec.y * vec.y + vec.z + vec.z;

		if (distance2 >= radius2)
			return;
		else {
				glm::ivec3 pos = glm::ivec3(chunk->GetPosition()) + glm::ivec3(0, m_chunkSize, 0);
				vec = pos - glm::ivec3(m_currentChunk);
				distance2 = vec.x * vec.x + vec.y * vec.y + vec.z + vec.z;
			if (chunk->NeighborGetTop() == nullptr && distance2 <= radius2) {
				CreateChunk(pos);
				RecursiveChunkUpdate(m_chunks[pos]);
			}
			else {}
		}
	}

	void ChunkManager::FillChunks()
	{
		
	}

	void ChunkManager::GenerateWorld()
	{
		
	}

	void ChunkManager::SetCurrentChunk(const glm::ivec3& pos)
	{
		glm::ivec3 offset(pos.x % m_chunkSize, pos.y % m_chunkSize, pos.z % m_chunkSize);
		glm::ivec3 chunkPos = pos - offset;
		if (m_chunks[chunkPos] == nullptr)
			CreateChunk(chunkPos);
		m_currentChunk = chunkPos;
	}

	bool ChunkManager::CreateChunk(const glm::ivec3& pos)
	{
		if (m_chunks[pos] == nullptr) {
			m_chunks[pos] = new Chunk(pos, m_chunkSize, m_maxWorldHeight);
			for (int k = 0; k < m_chunkSize; ++k) {
				for (int i = 0; i < m_chunkSize; ++i) {
					int height = int(m_maxWorldHeight - round(m_maxWorldHeight * m_noise.GetHeight(i + pos.x, k + pos.z)));
					for (int j = pos.y; j < height && j < pos.y + m_chunkSize; ++j) {
						m_chunks[pos]->SetVoxel(glm::vec3(i, j, k), 1);
					}
				}
			}
			return true;
		}
		return false;
	}
}