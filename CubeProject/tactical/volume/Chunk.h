#ifndef _CHUNK_H_
#define _CHUNK_H_

#include "../Common.h"
#include "../utils/Logger.h"

#include "Voxel.h"

#include "../render/structures/Mesh.h"
#include "../render/IRenderable3D.h"
#include "../render/geometry/Geometry.h"

#include "../math/AABB.h"

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
		class Chunk : public render::IRenderable3D
		{
		public:
			Chunk(glm::vec3 position, uint size, int maxHeight);
			~Chunk();

			void Draw(render::Shader& shader) override;

			bool IsSolid(const glm::vec3& position);
			bool IsModified();

			void Fill();
			void Empty();

			void SetVoxel(const glm::vec3& position, byte type);
			byte GetVoxel(const glm::vec3& position);

			void SetSize(uint size);
			inline uint GetSize() const { return m_size; }

			void SetPosition(const glm::vec3& position) { m_position = position; }
			inline glm::vec3 GetPosition() const { return m_position; }

			inline void SetMaxHeight(int maxHeight) { m_maxHeight = maxHeight; }
			inline int GetMaxHeight() const { return m_maxHeight; }

			inline VolumeIterator begin() { return m_voxels.begin(); }
			inline VolumeIterator end() { return m_voxels.end(); }

		protected:
			Chunk(); // can't create an undefined chunk
			Chunk(const Chunk& chunk); // No two chunks are equal

			void GenerateGeometry();

			// position is important when meshing the chunk, so we can get the correct position 
			// for each voxel
			glm::vec3 m_position;

			Volume m_voxels;
			uint m_size;
			int m_maxHeight;

			bool m_isVisible;

			render::Mesh<render::Vertex3f3f> m_mesh;
			math::AABB m_boudingBox;
		};
	}
}

#endif

