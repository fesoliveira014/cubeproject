#include "ChunkManager.h"

namespace tactical
{
	using namespace volume;
	
	ChunkManager::ChunkManager()
		: m_chunkSize(16), m_numChunks(0), m_maxWorldHeight(64)
	{
		m_currentChunk = glm::ivec3(0);
		m_noise = math::PerlinNoise2D(0.5, 1.0 / float(m_maxWorldHeight), 4, 10, 0);
		CreateChunk(m_currentChunk);
	}

	ChunkManager::ChunkManager(int seed)
		: m_chunkSize(16), m_numChunks(0), m_maxWorldHeight(64)
	{
		m_currentChunk = glm::ivec3(0);
		m_noise = math::PerlinNoise2D(0.5, 1.0 / float(m_maxWorldHeight), 4, 10, seed);
		CreateChunk(m_currentChunk);
	}

	ChunkManager::ChunkManager(const glm::ivec3& initialPosition, int seed)
		: m_chunkSize(16), m_numChunks(0), m_maxWorldHeight(64), m_currentChunk(initialPosition)
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
		glm::ivec3 offset(currentPos.x % m_chunkSize, currentPos.y % m_chunkSize, currentPos.z % m_chunkSize);
		glm::ivec3 chunkPos = currentPos - offset;

		if (currentPos != chunkPos) {
			SetCurrentChunk(currentPos);
			RecursiveChunkUpdate(m_chunks[currentPos]);
		}
	}

	void ChunkManager::RecursiveChunkUpdate(Chunk* chunk) {
		if (!IsWithinRadius(chunk->GetPosition()) && chunk != nullptr) {
			chunk->Unload();
			return;
		}
		else {
			glm::ivec3 top = chunk->GetPosition() + glm::vec3(0, m_chunkSize, 0);
			glm::ivec3 bottom = chunk->GetPosition() + glm::vec3(0, -m_chunkSize, 0);
			glm::ivec3 right = chunk->GetPosition() + glm::vec3(m_chunkSize, 0, 0);
			glm::ivec3 left = chunk->GetPosition() + glm::vec3(-m_chunkSize, 0, 0);
			glm::ivec3 front = chunk->GetPosition() + glm::vec3(0, 0, m_chunkSize);
			glm::ivec3 back = chunk->GetPosition() + glm::vec3(0, 0, -m_chunkSize);

			if (IsWithinRadius(top)) {
				if (chunk->NeighborGetTop() == nullptr) {
					CreateChunk(top);
				}
				RecursiveChunkUpdate(m_chunks[top]);
				m_chunks[top]->Load();
			}

			if (IsWithinRadius(bottom)) {
				if (chunk->NeighborGetBottom() == nullptr) {
					CreateChunk(bottom);
				}
				RecursiveChunkUpdate(m_chunks[bottom]);
				m_chunks[bottom]->Load();
			}

			if (IsWithinRadius(right)) {
				if (chunk->NeighborGetRight() == nullptr) {
					CreateChunk(right);
				}
				RecursiveChunkUpdate(m_chunks[right]);
				m_chunks[right]->Load();
			}

			if (IsWithinRadius(left)) {
				if (chunk->NeighborGetLeft() == nullptr) {
					CreateChunk(left);
				}
				RecursiveChunkUpdate(m_chunks[left]);
				m_chunks[left]->Load();
			}

			if (IsWithinRadius(front)) {
				if (chunk->NeighborGetFront() == nullptr) {
					CreateChunk(front);
				}
				RecursiveChunkUpdate(m_chunks[front]);
				m_chunks[front]->Load();
			}

			if (IsWithinRadius(back)) {
				if (chunk->NeighborGetBack() == nullptr) {
					CreateChunk(back);
				}
				RecursiveChunkUpdate(m_chunks[back]);
				m_chunks[back]->Load();
			}
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