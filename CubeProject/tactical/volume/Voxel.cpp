#include "Voxel.h"

namespace tactical
{
	namespace volume
	{
		Volume::Volume()
		{
			m_capacity = 32;
		}

		Volume::Volume(uint capacity)
		{
			m_capacity = capacity;
		}

		Volume::~Volume()
		{
			m_data.clear();
		}

		bool Volume::IsSolid(const glm::vec3& position)
		{
			return !(m_data.find(position) == m_data.end());
		}

		bool Volume::IsFull()
		{
			return (m_data.size() == m_capacity * m_capacity * m_capacity);
		}

		bool Volume::IsEmpty()
		{
			return (m_data.begin() == m_data.end());
		}

		bool Volume::IsModified()
		{
			return m_modified;
		}

		void Volume::Empty()
		{
			m_data.clear();
		}

		void Volume::Fill()
		{
			if (!IsFull()) {
				for (uint i = 0; i < m_capacity; ++i) {
					for (uint j = 0; j < m_capacity; ++j) {
						for (uint k = 0; k < m_capacity; ++k) {
							Set(glm::vec3(i, j, k), 1);
						}
					}
				}
				m_modified = true;
			}
		}

		const byte Volume::Get(const int x, const int y, const int z)
		{
			return Get(glm::vec3(x, y, z));
		}

		const byte Volume::Get(const glm::vec3& position)
		{
			if (IsSolid(position)) {
				return m_data[position].type;
			}

			else {
				return 0;
			}
		}

		void Volume::Set(const int x, const int y, const int z, byte type)
		{
			Set(glm::vec3(x, y, z), type);
		}

		void Volume::Set(const glm::vec3& position, byte type)
		{
			glm::vec3 key = glm::clamp(position, glm::vec3(0), glm::vec3((float)m_capacity));

			Voxel element;

			if (type > 0) {
				if (m_data[key].type != type) {
					element.type = type;
					m_data[key] = element;
					m_modified = true;
				}
			}

			else if (IsSolid(key)) {
				m_data.erase(key);
				m_modified = true;
			}
		}

		void Volume::Updated()
		{
			if (m_modified) { m_modified = false; }
		}

		///////////////////
		// Visibility check
		///////////////////

		bool Volume::TopVisible(const glm::vec3& position)
		{
			return !IsSolid(glm::vec3(position.x, position.y + 1, position.z));
		}

		bool Volume::BottomVisible(const glm::vec3& position)
		{
			return !IsSolid(glm::vec3(position.x, position.y - 1, position.z));
		}

		bool Volume::LeftVisible(const glm::vec3& position)
		{
			return !IsSolid(glm::vec3(position.x - 1, position.y, position.z));
		}

		bool Volume::RightVisible(const glm::vec3& position)
		{
			return !IsSolid(glm::vec3(position.x + 1, position.y, position.z));
		}

		bool Volume::FrontVisible(const glm::vec3& position)
		{
			return !IsSolid(glm::vec3(position.x, position.y, position.z + 1));
		}

		bool Volume::BackVisible(const glm::vec3& position)
		{
			return !IsSolid(glm::vec3(position.x, position.y, position.z - 1));
		}


		/////////////////////
		// Mesh merging check
		/////////////////////

		bool Volume::IsTopFaceFull()
		{
			uint y = m_capacity - 1;

			for (uint x = 0; x < m_capacity; ++x) {
				for (uint z = 0; z < m_capacity; ++z) {
					if (!IsSolid(glm::vec3(x, y, z))) {
						return false;
					}
				}
			}

			return true;
		}

		bool Volume::IsBottomFaceFull()
		{
			uint y = 0;

			for (uint x = 0; x < m_capacity; ++x) {
				for (uint z = 0; z < m_capacity; ++z) {
					if (!IsSolid(glm::vec3(x, y, z))) {
						return false;
					}
				}
			}

			return true;
		}

		bool Volume::IsLeftFaceFull()
		{
			uint x = 0;

			for (uint y = 0; y < m_capacity; ++y) {
				for (uint z = 0; z < m_capacity; ++z) {
					if (!IsSolid(glm::vec3(x, y, z))) {
						return false;
					}
				}
			}

			return true;
		}

		bool Volume::IsRightFaceFull()
		{
			uint x = m_capacity - 1;

			for (uint y = 0; y < m_capacity; ++y) {
				for (uint z = 0; z < m_capacity; ++z) {
					if (!IsSolid(glm::vec3(x, y, z))) {
						return false;
					}
				}
			}

			return true;
		}

		bool Volume::IsFrontFaceFull()
		{
			uint z = m_capacity - 1;

			for (uint x = 0; x < m_capacity; ++x) {
				for (uint y = 0; y < m_capacity; ++y) {
					if (!IsSolid(glm::vec3(x, y, z))) {
						return false;
					}
				}
			}

			return true;
		}

		bool Volume::IsBackFaceFull()
		{
			uint z = 0;

			for (uint x = 0; x < m_capacity; ++x) {
				for (uint y = 0; y < m_capacity; ++y) {
					if (!IsSolid(glm::vec3(x, y, z))) {
						return false;
					}
				}
			}

			return true;
		}
	}
}