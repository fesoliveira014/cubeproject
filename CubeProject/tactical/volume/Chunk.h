#ifndef _CHUNK_H_
#define _CHUNK_H_

#include "../Common.h"
#include "../utils/Logger.h"

#include "Voxel.h"

#include "../render/structures/Mesh.h"
#include "../render/IRenderable3D.h"
#include "../render/geometry/Geometry.h"

#include "../math/AABB.h"
#include "../math/Ray.h"
/*
Chunks are blocks of voxels representing a cubic region of the world. Each chunk is rendered indepen-
dantly, and each has it's own geometry. Voxel coordinates are in "chunk space", that is, are numbered 
from (0,0,0) to (n-1, n-1, n-1), where 'n' is the number of voxels, and need to be converted to world
coodinates during mesh generation.
*/

namespace tactical
{
	namespace volume
	{
		// Chunks are containers for volumes that are renderable.
		// It is here that the geometry for the chunk is generated based on the
		// active voxels, taking in consideration differents types of blocks.
		class Chunk : public render::IRenderable3D
		{
		public:
			Chunk(glm::vec3 position, uint size, int maxHeight);
			~Chunk();

			void Draw(render::Shader& shader) override;

			// Checks if chunk is full
			bool IsSolid(const glm::vec3& position);

			// Checks if chunk has been modified (dirty)
			bool IsModified();

			void Update();
			void UpdateVisibility();
			
			void SetDirty() { m_voxels.SetModified(); }
			bool NeedsUpdate() { return m_voxels.IsModified(); }
			void Updated() { m_voxels.Updated(); }

			Volume* GetVolume() { return &m_voxels; }

			// Methods to fill and empty the chunk
			void Fill();
			void Empty();

			// Setters and getters
			void SetVoxel(const glm::vec3& position, byte type);
			void SetVoxel(const int x, const int y, const int z, byte type);

			byte GetVoxel(const glm::vec3& position);
			byte GetVoxel(const int x, const int y, const int z);

			void SetSize(int size);
			inline int GetSize() const { return m_size; }

			void SetPosition(const glm::vec3& position) { m_position = position; }
			inline glm::vec3 GetPosition() const { return m_position; }

			inline void SetMaxHeight(int maxHeight) { m_maxHeight = maxHeight; }
			inline int GetMaxHeight() const { return m_maxHeight; }

			byte GetAmbientOcclusion(const glm::vec3& vertice);

			inline render::Mesh<render::Vertex3f3f4f>* GetMesh() { return &m_mesh; }

			// Iterators
			inline VolumeIterator begin() { return m_voxels.begin(); }
			inline VolumeIterator end() { return m_voxels.end(); }

			// neighborhood
			std::shared_ptr<Chunk> NeighborGetTop()    { return m_neighbors[0]; }
			std::shared_ptr<Chunk> NeighborGetBottom() { return m_neighbors[1]; }
			std::shared_ptr<Chunk> NeighborGetRight()  { return m_neighbors[2]; }
			std::shared_ptr<Chunk> NeighborGetLeft()   { return m_neighbors[3]; }
			std::shared_ptr<Chunk> NeighborGetFront()  { return m_neighbors[4]; }
			std::shared_ptr<Chunk> NeighborGetBack()   { return m_neighbors[5]; }

			void NeighborSetTop(std::shared_ptr<Chunk> chunk)    { m_neighbors[0] = chunk; }
			void NeighborSetBottom(std::shared_ptr<Chunk> chunk) { m_neighbors[1] = chunk; }
			void NeighborSetRight(std::shared_ptr<Chunk> chunk)  { m_neighbors[2] = chunk; }
			void NeighborSetLeft(std::shared_ptr<Chunk> chunk)   { m_neighbors[3] = chunk; }
			void NeighborSetFront(std::shared_ptr<Chunk> chunk)  { m_neighbors[4] = chunk; }
			void NeighborSetBack(std::shared_ptr<Chunk> chunk)	 { m_neighbors[5] = chunk; }

			int GetNumOfNeighbors();

			bool IsFaceVisible(const glm::vec3& pos, render::geometry::Face face);

			void Load();
			void Unload();

			inline bool IsActive() const { return m_isActive; }

		protected:
			Chunk(); // can't create an undefined chunk
			Chunk(const Chunk& chunk); // No two chunks are equal

			glm::ivec3 GetGridPosition(const glm::vec3& pos);

			// position is important when meshing the chunk, so we can get the correct position 
			// for each voxel
			glm::vec3 m_position;

			Volume m_voxels;
			int m_size;
			int m_maxHeight;

			bool m_isVisible;
			bool m_isActive;

			render::Mesh<render::Vertex3f3f4f> m_mesh;

			std::shared_ptr<Chunk> m_neighbors[6]; // 0 - top, 1 - bottom, 2 - left, 3 - right, 4 - front, 5 - back
		};
	}
}

#endif

