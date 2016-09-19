#include "ChunkManager.h"

namespace tactical
{
	using namespace volume;

	ChunkManager::ChunkManager(render::Renderer* pRenderer, const glm::vec3& worldDimension, int seed) :
		m_chunkSize(16),
		m_maxWorldHeight(64),
		m_chunkLoadingRadius(4),
		m_worldDimensions(worldDimension),
		m_pRenderer(pRenderer)
	{
		
		m_currentChunk = glm::vec3(0);
		Initialize();
		
	}

	ChunkManager::~ChunkManager()
	{
		for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
			(iter)->second.reset();
		}

		m_pRenderer = nullptr;
	}

	void ChunkManager::Initialize()
	{
		auto chunkDeleter = [](Chunk* chunk) {
			delete chunk;
		};

		glm::vec3 key, neighKey, pos;
		for (int k = 0; k < m_worldDimensions.z; ++k) {
			for (int j = 0; j < m_worldDimensions.y; ++j) {
				for (int i = 0; i < m_worldDimensions.x; ++i) {
					key.x = i; key.y = j; key.z = k;
					pos = GridCoordsToWorldCoords(key);
					m_chunks[key] = std::shared_ptr<Chunk> (new Chunk(pos, m_chunkSize, m_maxWorldHeight));
				}
			}
		}

		for (int k = 0; k < m_worldDimensions.z; ++k) {
			for (int j = 0; j < m_worldDimensions.y; ++j) {
				for (int i = 0; i < m_worldDimensions.x; ++i) {
					key.x = i; key.y = j; key.z = k;

					m_chunks[key]->NeighborSetTop(nullptr);
					m_chunks[key]->NeighborSetBottom(nullptr);
					m_chunks[key]->NeighborSetRight(nullptr);
					m_chunks[key]->NeighborSetLeft(nullptr);
					m_chunks[key]->NeighborSetFront(nullptr);
					m_chunks[key]->NeighborSetBack(nullptr);

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
						m_chunks[key]->NeighborSetBack(m_chunks[neighKey]);
					}

					neighKey = key;
					if (k > 0) {
						neighKey.z -= 1;
						m_chunks[key]->NeighborSetFront(m_chunks[neighKey]);
					}
				}
			}
		}

		m_baseFlat.SetFrequency(2.0f);

		m_flat.SetSourceModule(0, m_baseFlat);
		m_flat.SetScale(0.125);
		m_flat.SetBias(0.75);

		m_perlin.SetFrequency(0.02);
		m_perlin.SetPersistence(0.5);
		m_perlin.SetOctaveCount(8);

		m_selector.SetSourceModule(0, m_flat);
		m_selector.SetSourceModule(1, m_mountains);
		m_selector.SetControlModule(m_perlin);
		m_selector.SetBounds(0.0, 100.0);
		m_selector.SetEdgeFalloff(0.125);

		m_final.SetSourceModule(0, m_selector);
		m_final.SetFrequency(4.0f);
		m_final.SetPower(0.125f);

		

		noise::utils::NoiseMapBuilderPlane heightMapBuilder;
		heightMapBuilder.SetSourceModule(m_final);
		heightMapBuilder.SetDestNoiseMap(m_heightMap);
		heightMapBuilder.SetDestSize(m_worldDimensions.x * m_chunkSize, m_worldDimensions.z * m_chunkSize);
		heightMapBuilder.SetBounds(0.0, 12.8, 0.0, 12.8);
		heightMapBuilder.Build();

		m_fastnoise.SetSeed(1337);
		m_fastnoise.SetFrequency(0.08f);
		m_fastnoise.SetInterp(FastNoise::Interp::Hermite);
		m_fastnoise.SetNoiseType(FastNoise::NoiseType::GradientFractal);
		m_fastnoise.SetFractalOctaves(4);
		m_fastnoise.SetFractalLacunarity(2.0f);
		m_fastnoise.SetFractalType(FastNoise::FractalType::FBM);
		m_fastnoise.SetFractalGain(0.5f);


		m_meshNeedsUpdate = true;
	}

	void ChunkManager::Draw(std::string shaderID)
	{
		if (!m_chunks.empty()) {
			for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
				m_pRenderer->Render(static_cast<std::shared_ptr<render::IRenderable3D>>((*iter).second), shaderID);
			}
		}
	}

	void ChunkManager::UpdateChunks(const glm::vec3& currentPos)
	{
		if (!m_chunks.empty() && m_meshNeedsUpdate) {
			for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
				if ((*iter).second->NeedsUpdate())
					mesher::GenerateChunkMesh(*(*iter).second, mesher::GREEDY);
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

	void ChunkManager::FillWithPyramids()
	{
		if (!m_chunks.empty()) {
			for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
				GeneratePyramid(*iter->second);
			}
		}
	}

	void ChunkManager::GeneratePyramid(Chunk &chunk)
	{
		for (int j = 0; j < m_chunkSize / 2; ++j) {
			for (int k = 0; k < m_chunkSize / 2; ++k) {
				for (int i = 0; i < m_chunkSize / 2; ++i) {
					if ((i >= j && k >= j)) {
						chunk.SetVoxel(i, j, k, 1);
						chunk.SetVoxel(m_chunkSize - 1 - i, j, k, 1);
						chunk.SetVoxel(m_chunkSize - 1 - i, j, m_chunkSize - 1 - k, 1);
						chunk.SetVoxel(i, j, m_chunkSize - 1 - k, 1);
						
					}
					/*if ((m_chunkSize - i >= j && k >= j)) {
						chunk.SetVoxel(i, j, k, 1);
						
					}*/
				}
			}
		}
	}

	void ChunkManager::GenerateWorld()
	{	
		if (!m_chunks.empty()) {
			for (ChunkIterator iter = m_chunks.begin(); iter != m_chunks.end(); ++iter) {
				for (int k = 0; k < m_chunkSize; ++k) {
					
					for (int i = 0; i < m_chunkSize; ++i) {
						int x = (int)(*iter).second->GetPosition().x;
						int y = (int)(*iter).second->GetPosition().y;
						int z = (int)(*iter).second->GetPosition().z;

						double noise =  m_heightMap.GetValue((i + x), (k + z));
						//float noise = m_fastnoise.GetGradientFractal(i + x, k + z);
						noise = (noise + 1) * 0.5f;
						int height = (int)((m_worldDimensions.y - 1) * m_chunkSize - y) + m_chunkSize * noise;
						height = height <= 0 ? 1 : height > m_chunkSize ? m_chunkSize : height;


						for (int j = 0; j < height; ++j) {
							if (j < 1)
								(*iter).second->SetVoxel(glm::vec3(i,j,k), 4);
							else if (j >= 1 && j < 16)
								(*iter).second->SetVoxel(glm::vec3(i, j, k), 2);
							else if (j >= 16 && j < 27)
								(*iter).second->SetVoxel(glm::vec3(i, j, k), 5);
							else if (j >= 27)
								(*iter).second->SetVoxel(glm::vec3(i, j, k), 3);

						}
					}
				}
			}
		}
	}

	void ChunkManager::SetCurrentChunk(const glm::vec3& pos)
	{

	}

	math::RayCastResult ChunkManager::GetRayVoxelIntersection(math::Ray & ray, const glm::vec3& pos, float pickRadius)
	{
		math::RayCastResult result;
		result.hit = false;

		if (!m_chunks.empty()) 
			result = math::PickVoxel(ray, m_chunks, 64, m_chunkSize);

		return result;
	}

	bool ChunkManager::IsWithinRadius(const glm::vec3& position)
	{
		glm::vec3 vec = position - m_currentChunk;
		int radius = m_chunkLoadingRadius * m_chunkSize;
		int radius2 = radius * radius;
		int distance2 = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;

		return (distance2 < (float)radius2);
	}

	glm::vec3 ChunkManager::GridCoordsToWorldCoords(const glm::vec3 & pos)
	{
		return (float)m_chunkSize * pos;
	}

	glm::vec3 ChunkManager::World2Chunk(const glm::vec3& pos) 
	{
		glm::vec3 offset(glm::vec3(glm::fmod(pos, (float)m_chunkSize)));
		glm::vec3 position = pos - offset;
		position = glm::vec3(position.x / m_chunkSize, position.y / m_chunkSize, position.z / m_chunkSize);

		return position;
	};

	glm::vec3 ChunkManager::World2Voxel(const glm::vec3& pos) 
	{
		return glm::vec3(glm::fmod(pos, (float)m_chunkSize));
	};

	byte ChunkManager::GetVoxel(const glm::vec3& pos) 
	{
		// find chunk index associated with position
		glm::vec3 chunkIndex = World2Chunk(pos);

		// check if chunk exists
		if (m_chunks.find(chunkIndex) != m_chunks.end()) {
			// find voxel position on chunk coordinates
			glm::vec3 voxelPos = World2Voxel(pos);
			return m_chunks[chunkIndex]->GetVoxel(voxelPos);
		}

		return byte(0);
	};

	void ChunkManager::SetVoxel(const glm::vec3& pos, byte type)
	{
		// find chunk index associated with position
		glm::vec3 chunkIndex = World2Chunk(pos);

		// check if chunk exists
		if (m_chunks.find(chunkIndex) != m_chunks.end()) {
			// find voxel position on chunk coordinates
			glm::vec3 voxelPos = World2Voxel(pos);
			m_chunks[chunkIndex]->SetVoxel(pos, type);
			m_meshNeedsUpdate = true;
		}
	};
}