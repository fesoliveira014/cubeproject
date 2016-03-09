#include "ChunkManager.h"

namespace tactical
{
	using namespace volume;

	ChunkManager::ChunkManager(const glm::ivec3& worldDimension, int seed)
		: m_chunkSize(16), m_maxWorldHeight(64), m_chunkLoadingRadius(4), m_worldDimensions(worldDimension)
	{
		m_currentChunk = glm::ivec3(0);
		m_noise = math::PerlinNoise2D(0.5, 1.0 / float(m_maxWorldHeight), 4, 10, seed);
		CreateChunk(m_currentChunk);
	}

	ChunkManager::ChunkManager(const glm::ivec3& worldDimension, const glm::ivec3& initialPosition, int seed)
		: m_chunkSize(16), m_maxWorldHeight(64), m_currentChunk(initialPosition), m_chunkLoadingRadius(4), m_worldDimensions(worldDimension)
	{
		m_noise = math::PerlinNoise2D(0.5, 1.0 / float(m_maxWorldHeight), 4, 10, seed);
		CreateChunk(m_currentChunk);
	}

	ChunkManager::~ChunkManager()
	{
		for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
			delete iter->second;
		}
	}

	void ChunkManager::Initialize()
	{
		glm::ivec3 key;
		for (int i = 0; i < m_worldDimensions.x; ++i) {
			for (int k = 0; k < m_worldDimensions.z; ++k) {
				for (int j = 0; j < m_worldDimensions.y; ++j) {
					key.x = i * m_chunkSize; 
					key.y = j * m_chunkSize; 
					key.z = k * m_chunkSize;
					CreateChunk(key + m_currentChunk);
				}
			}
		}

		for (int i = 0; i < m_chunkLoadingRadius; i += m_chunkSize) {
			for (int k = 0; k < m_chunkLoadingRadius; k += m_chunkSize) {
				for (int j = 0; j < m_maxWorldHeight / m_chunkSize; j += m_chunkSize) {
					key.x = i; key.y = j; key.z = k;
					m_chunks[key + m_currentChunk]->NeighborSetTop(m_chunks[glm::ivec3(i, j + m_chunkSize, k) + m_currentChunk]);
					m_chunks[key + m_currentChunk]->NeighborSetBottom(m_chunks[glm::ivec3(i, j - m_chunkSize, k) + m_currentChunk]);
					m_chunks[key + m_currentChunk]->NeighborSetRight(m_chunks[glm::ivec3(i + m_chunkSize, j, k) + m_currentChunk]);
					m_chunks[key + m_currentChunk]->NeighborSetLeft(m_chunks[glm::ivec3(i - m_chunkSize, j, k) + m_currentChunk]);
					m_chunks[key + m_currentChunk]->NeighborSetFront(m_chunks[glm::ivec3(i, j, k + m_chunkSize) + m_currentChunk]);
					m_chunks[key + m_currentChunk]->NeighborSetBack(m_chunks[glm::ivec3(i, j, k - m_chunkSize) + m_currentChunk]);
				}
			}
		}

	}

	void ChunkManager::Draw(render::Shader& shader)
	{
		Draw(m_chunks[m_currentChunk], shader);
	}

	void ChunkManager::Draw(Chunk* chunk, render::Shader& shader)
	{
		if (chunk != nullptr) {
			chunk->Draw(shader);
			Draw(chunk->NeighborGetTop(), shader);
			Draw(chunk->NeighborGetBottom(), shader);
			Draw(chunk->NeighborGetRight(), shader);
			Draw(chunk->NeighborGetLeft(), shader);
			Draw(chunk->NeighborGetFront(), shader);
			Draw(chunk->NeighborGetBack(), shader);
		}
	}

	void ChunkManager::UpdateChunks(const glm::ivec3& currentPos)
	{

	}

	void ChunkManager::RecursiveChunkUpdate(Chunk* chunk) 
	{
		
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
		if (m_chunks[pos] == nullptr && (pos.y >= 0 && pos.y < m_maxWorldHeight)) {
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

	void ChunkManager::SetNoise(double persistance, double frequency, double amplitude, int octaves, int seed)
	{
		m_noise.Set(persistance, frequency, amplitude, octaves, seed);
	}

	bool ChunkManager::IsWithinRadius(const glm::ivec3& position)
	{
		glm::ivec3 vec = position - m_currentChunk;
		int radius = m_chunkLoadingRadius * m_chunkSize;
		int radius2 = radius * radius;
		int distance2 = vec.x * vec.x + vec.y * vec.y + vec.z + vec.z;

		return (distance2 < radius2);
	}
}