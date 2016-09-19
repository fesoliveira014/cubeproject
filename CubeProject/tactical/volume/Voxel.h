#ifndef _VOXEL_H_
#define _VOXEL_H_

#include "../Common.h"

namespace tactical
{
	namespace volume
	{
		struct Voxel
		{
			byte type;
		};

		using VolumeData = std::unordered_map<glm::vec3, Voxel,
			std::hash<glm::vec3>, std::equal_to<glm::vec3>>;
		using VolumeIterator = std::unordered_map<glm::vec3, Voxel,
			std::hash<glm::vec3>, std::equal_to<glm::vec3>>::iterator;

		class Volume
		{
		public:
			Volume();
			Volume(uint capacity);
			~Volume();

			bool IsSolid(const glm::vec3& position);
			bool IsEmpty();
			bool IsFull();

			void SetModified() { m_modified = true; }
			bool IsModified();
			void Updated();

			// Methods for checking face visibility
			bool TopVisible(const glm::vec3& position);
			bool BottomVisible(const glm::vec3& position);
			bool LeftVisible(const glm::vec3& position);
			bool RightVisible(const glm::vec3& position);
			bool FrontVisible(const glm::vec3& position);
			bool BackVisible(const glm::vec3& position);

			// Helper methods for mesh merging
			bool IsTopFaceFull();
			bool IsBottomFaceFull();
			bool IsLeftFaceFull();
			bool IsRightFaceFull();
			bool IsFrontFaceFull();
			bool IsBackFaceFull();

			void Empty();
			void Fill();

			const byte Get(const int x, const int y, const int z);
			const byte Get(const glm::vec3& position);

			void Set(const int x, const int y, const int z, byte type);
			void Set(const glm::vec3& position, byte type);

			void SetCapacity(uint capacity) { m_capacity = capacity; }

			VolumeIterator begin() { return m_data.begin(); }
			VolumeIterator end() { return m_data.end(); }

			inline GLsizei GetNumOfVoxels() const { return (GLsizei)m_data.size(); }

		protected:
			uint m_capacity;
			bool m_modified;

			VolumeData m_data;

		};
	}
}

#endif