#include "ChunkManager.h"

namespace tactical
{
	using namespace volume;

	ChunkManager::ChunkManager(render::Renderer* pRenderer, const glm::ivec3& worldDimension, int seed) :
		m_chunkSize(32),
		m_maxWorldHeight(32),
		m_chunkLoadingRadius(4),
		m_worldDimensions(worldDimension),
		m_pRenderer(pRenderer)
	{
		
		m_currentChunk = glm::ivec3(0);
		Initialize();
		
	}

	ChunkManager::~ChunkManager()
	{
		for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
			delete iter->second;
		}

		m_pRenderer = nullptr;
	}

	void ChunkManager::Initialize()
	{
		glm::ivec3 key, neighKey, pos;
		for (int k = 0; k < m_worldDimensions.z; ++k) {
			for (int j = 0; j < m_worldDimensions.y; ++j) {
				for (int i = 0; i < m_worldDimensions.x; ++i) {
					key.x = i; key.y = j; key.z = k;
					pos = GridCoordsToWorldCoords(key);
					m_chunks[key] = new Chunk(pos, m_chunkSize, m_maxWorldHeight);
				}
			}
		}

		for (int k = 0; k < m_worldDimensions.z; ++k) {
			for (int j = 0; j < m_worldDimensions.y; ++j) {
				for (int i = 0; i < m_worldDimensions.x; ++i) {
					key.x = i; key.y = j; key.z = k;

					m_chunks[key]->NeighborSetTop(NULL);
					m_chunks[key]->NeighborSetBottom(NULL);
					m_chunks[key]->NeighborSetRight(NULL);
					m_chunks[key]->NeighborSetLeft(NULL);
					m_chunks[key]->NeighborSetFront(NULL);
					m_chunks[key]->NeighborSetBack(NULL);

					neighKey = key;
					if (j < m_worldDimensions.y - 1) {
						neighKey.y += 1;
						m_chunks[key]->NeighborSetTop(m_chunks[neighKey]);
					}
					
					neighKey = key;
					if (j > 0) {
						neighKey.y -= 1;
						m_chunks[key]->NeighborSetBottom(m_chunks[neighKey]);
					}

					neighKey = key;
					if (i < m_worldDimensions.x - 1) {
						neighKey.x += 1;
						m_chunks[key]->NeighborSetRight(m_chunks[neighKey]);
					}

					neighKey = key;
					if (i > 0) {
						neighKey.x -= 1;
						m_chunks[key]->NeighborSetLeft(m_chunks[neighKey]);
					}

					neighKey = key;
					if (k < m_worldDimensions.z - 1) {
						neighKey.z += 1;
						m_chunks[key]->NeighborSetFront(m_chunks[neighKey]);
					}

					neighKey = key;
					if (k > 0) {
						neighKey.z -= 1;
						m_chunks[key]->NeighborSetBack(m_chunks[neighKey]);
					}
				}
			}
		}

		m_baseFlat.SetFrequency(2.0f);

		m_flat.SetSourceModule(0, m_baseFlat);
		m_flat.SetScale(0.125);
		m_flat.SetBias(-0.75);

		m_perlin.SetFrequency(0.25);
		m_perlin.SetPersistence(0.5);
		m_perlin.SetOctaveCount(6);

		m_selector.SetSourceModule(0, m_flat);
		m_selector.SetSourceModule(1, m_mountains);
		m_selector.SetControlModule(m_perlin);
		m_selector.SetBounds(0.0, 1000.0);
		m_selector.SetEdgeFalloff(0.125);

		m_final.SetSourceModule(0, m_selector);
		m_final.SetFrequency(4.0f);
		m_final.SetPower(0.125f);

		noise::utils::NoiseMapBuilderPlane heightMapBuilder;
		heightMapBuilder.SetSourceModule(m_final);
		heightMapBuilder.SetDestNoiseMap(m_heightMap);
		heightMapBuilder.SetDestSize(m_worldDimensions.x * m_chunkSize, m_worldDimensions.z * m_chunkSize);
		heightMapBuilder.SetBounds(1.0, 4.0, 1.0, 4.0);
		heightMapBuilder.Build();

		m_meshNeedsUpdate = true;
	}

	/*void ChunkManager::Draw(render::Shader& shader)
	{
		Draw(m_chunks[m_currentChunk], shader);
	}*/

	void ChunkManager::Draw(std::string shaderID)
	{
		if (!m_chunks.empty()) {
			for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
				m_pRenderer->Render((*iter).second, shaderID);
			}
		}
	}

	void ChunkManager::UpdateChunks(const glm::ivec3& currentPos)
	{
		if (!m_chunks.empty() && m_meshNeedsUpdate) {
			for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
				(*iter).second->GenerateGeometry();
			}
			m_meshNeedsUpdate = false;
		}
	}

	void ChunkManager::FillChunks()
	{
		if (!m_chunks.empty()) {
			for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
				(*iter).second->Fill();
			}
		}
	}

	void ChunkManager::GenerateWorld()
	{
		noise::model::Plane planeModel;
		planeModel.SetModule(m_final);

		if (!m_chunks.empty()) {
			for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
				for (int k = 0; k < m_chunkSize; ++k) {
					
					for (int i = 0; i < m_chunkSize; ++i) {
						int x = (*iter).second->GetPosition().x;
						int y = (*iter).second->GetPosition().y;
						int z = (*iter).second->GetPosition().z;

						
						double noise =  m_heightMap.GetValue((i + x), (k + z));
						noise = (noise + 1) * 0.5f;
						int height = ((m_worldDimensions.y - 1) * m_chunkSize - y) + m_chunkSize * noise;
						height = height <= 0 ? 1 : height > m_chunkSize ? m_chunkSize : height;


						for (int j = 0; j < height; ++j) {
							(*iter).second->SetVoxel(glm::vec3(i,j,k), 1);
						}
					}
				}
			}
		}
	}

	void ChunkManager::SetCurrentChunk(const glm::ivec3& pos)
	{

	}

	bool ChunkManager::IsWithinRadius(const glm::ivec3& position)
	{
		glm::ivec3 vec = position - m_currentChunk;
		int radius = m_chunkLoadingRadius * m_chunkSize;
		int radius2 = radius * radius;
		int distance2 = vec.x * vec.x + vec.y * vec.y + vec.z + vec.z;

		return (distance2 < radius2);
	}

	glm::ivec3 ChunkManager::WorldCoordsToGridCoords(const glm::ivec3& pos) 
	{
		glm::ivec3 offset(pos.x % m_chunkSize, pos.y % m_chunkSize, pos.z % m_chunkSize);
		glm::ivec3 chunkPos = pos - offset;
		chunkPos = chunkPos / m_chunkSize;

		return chunkPos;
	}

	glm::ivec3 ChunkManager::GridCoordsToWorldCoords(const glm::ivec3 & pos)
	{
		return pos * m_chunkSize;
	}
}