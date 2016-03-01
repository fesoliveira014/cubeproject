#include "ChunkManager.h"

namespace tactical
{
	using namespace volume;
	
	ChunkManager::ChunkManager()
		: m_chunkSize(16), m_numChunks(0)
	{
		m_currentChunk = glm::ivec3(0);
		m_noise = math::PerlinNoise2D(0.5, 1.0 / 64.0, 4, 10, 0);
	}

	ChunkManager::~ChunkManager()
	{
		for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
			delete iter->second;
		}
	}

	void ChunkManager::FillChunks()
	{
		
	}

	void ChunkManager::GenerateWorld()
	{
		
	}


}