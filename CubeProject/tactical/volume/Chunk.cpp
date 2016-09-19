#include "Chunk.h"

namespace tactical
{
	namespace volume
	{
		Chunk::Chunk(glm::vec3 position, uint size, int maxHeight)
		{
			m_position = position;
			m_size = size;
			m_maxHeight = maxHeight;

			m_voxels.SetCapacity(size);
			m_isActive = true;
			m_isVisible = true;

			m_boudingBox = math::AABB(m_position, 
				glm::vec3(m_position.x + m_size, m_position.y + m_size, m_position.z + m_size));

			for (int i = 0; i < 6; i++)
				m_neighbors[i] = nullptr;
			
			Load();
		}

		Chunk::~Chunk()
		{
			delete m_mesh.ibo;
			delete m_mesh.vao;
		}

		void Chunk::Draw(render::Shader& shader)
		{
			if (m_isVisible) {
				glm::mat4 model = glm::translate(glm::mat4(1.0f), m_position);

				m_mesh.vao->Bind();
				m_mesh.ibo->Bind();
				if (!shader.IsEnabled())
					shader.Enable();

				shader.SetUniformMat4fv("model", model);
				glDrawElements(GL_TRIANGLES, (GLsizei)m_mesh.ibo->GetCount(), GL_UNSIGNED_INT, 0);

				m_mesh.ibo->Unbind();
				m_mesh.vao->Unbind();
			}
		}

		bool Chunk::IsSolid(const glm::vec3& position)
		{
			return m_voxels.IsFull();
		}

		bool Chunk::IsModified()
		{
			return m_voxels.IsModified();
		}

		void Chunk::Fill()
		{
			m_voxels.Fill();
		}

		void Chunk::Empty()
		{
			m_voxels.Empty();
		}

		void Chunk::SetVoxel(const glm::vec3& position, byte type)
		{
			glm::vec3 pos;

			pos.x = (position.x < m_size) && (position.x >= 0) ? position.x : position.x - m_position.x;
			pos.y = (position.y < m_size) && (position.y >= 0) ? position.y : position.y - m_position.y;
			pos.z = (position.z < m_size) && (position.z >= 0) ? position.z : position.z - m_position.z;

			if (pos.y == m_size - 1 && m_neighbors[0] != nullptr && m_neighbors[0]->GetVoxel(pos.x, 0, pos.z)) {
				m_neighbors[0]->SetDirty();
			}
			if (pos.y == 0 && m_neighbors[1] != nullptr && m_neighbors[1]->GetVoxel(pos.x, m_size - 1, pos.z)) {
				m_neighbors[1]->SetDirty();
			}
			if (pos.x == m_size - 1 && m_neighbors[2] != nullptr && m_neighbors[2]->GetVoxel(0, pos.y, pos.z)) {
				m_neighbors[2]->SetDirty();
			}
			if (pos.x == 0 && m_neighbors[3] != nullptr && m_neighbors[3]->GetVoxel(m_size - 1, pos.y, pos.z)) {
				m_neighbors[3]->SetDirty();
			}
			if (pos.z == 0 && m_neighbors[4] != nullptr && m_neighbors[4]->GetVoxel(pos.x, pos.y, m_size - 1)) {
				m_neighbors[4]->SetDirty();
			}
			if (pos.z == m_size - 1 && m_neighbors[5] != nullptr && m_neighbors[5]->GetVoxel(pos.x, pos.y, 0)) {
				m_neighbors[5]->SetDirty();
			}

			m_voxels.Set(pos, type);
		}

		void Chunk::SetVoxel(const int x, const int y, const int z, byte type)
		{
			SetVoxel(glm::vec3(x, y, z), type);
		}

		byte Chunk::GetVoxel(const glm::vec3& position)
		{
			glm::vec3 pos;

			pos.x = (position.x < m_size) && (position.x >= 0) ? position.x : position.x - m_position.x;
			pos.y = (position.y < m_size) && (position.y >= 0) ? position.y : position.y - m_position.y;
			pos.z = (position.z < m_size) && (position.z >= 0) ? position.z : position.z - m_position.z;

			return m_voxels.Get(pos);
		}

		byte Chunk::GetVoxel(const int x, const int y, const int z)
		{
			return GetVoxel(glm::vec3(x, y, z));
		}

		void Chunk::SetSize(int size)
		{
			m_size = size;
			m_voxels.SetCapacity(size);
		}

		int Chunk::GetNumOfNeighbors()
		{
			int ret = 0;
			for (int i = 0; i < 6; i++) {
				if (m_neighbors[i] != nullptr)
					ret++;
			}
			return ret;
		}

		bool Chunk::IsFaceVisible(const glm::vec3 & pos, render::geometry::Face face)
		{
			byte neighbor = 0;
			if (face.face == render::geometry::Face::TOP) {
				if (pos.y == m_size - 1) {
					if (m_neighbors[0] != nullptr) 
						neighbor = m_neighbors[0]->GetVoxel(glm::vec3(pos.x, 0, pos.z));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(0, 1, 0));
				}
			}

			else if (face.face == render::geometry::Face::BOTTOM) {
				if (pos.y == 0) {
					if (m_neighbors[1] != nullptr) 
						neighbor = m_neighbors[1]->GetVoxel(glm::vec3(pos.x, m_size-1, pos.z));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(0, -1, 0));
				}
			}

			else if (face.face == render::geometry::Face::RIGHT) {
				if (pos.x == m_size - 1) {
					if (m_neighbors[2] != nullptr)
						neighbor = m_neighbors[2]->GetVoxel(glm::vec3(0, pos.y, pos.z));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(1, 0, 0));
				}
			}

			else if (face.face == render::geometry::Face::LEFT) {
				if (pos.x == 0) {
					if (m_neighbors[3] != nullptr) 
						neighbor = m_neighbors[3]->GetVoxel(glm::vec3(m_size-1, pos.y, pos.z));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(-1, 0, 0));
				}
			}

			else if (face.face == render::geometry::Face::FRONT) {
				if (pos.z == 0) {
					if (m_neighbors[4] != nullptr)
						neighbor = m_neighbors[4]->GetVoxel(glm::vec3(pos.x, pos.y, m_size-1));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(0, 0, -1));
				}
			}

			else if (face.face == render::geometry::Face::BACK) {
				if (pos.z == m_size - 1) {
					if (m_neighbors[5] != nullptr) 
						neighbor = m_neighbors[5]->GetVoxel(glm::vec3(pos.x, pos.y, 0));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(0, 0, 1));
				}
			}

			return (!neighbor ? true : false);
		}

		void Chunk::Load()
		{
			m_isActive = true;
		}

		void Chunk::Unload()
		{
			delete m_mesh.vao;
			delete m_mesh.ibo;
			m_mesh.vertices.clear();
			m_mesh.indices.clear();

			m_isActive = false;
			m_isVisible = false;
		}

		glm::ivec3 Chunk::GetGridPosition(const glm::vec3 & pos)
		{
			return glm::ivec3(pos);
		}

		void Chunk::Update()
		{
			UpdateVisibility();
		}
		
		void Chunk::UpdateVisibility()
		{
			for (int i = 0; i < 6; ++i) {
				if (m_neighbors[i] == nullptr || !m_neighbors[i]->IsActive()) {
					m_isVisible = true;
					return;
				}
			}
			m_isVisible = false;
		}
	}
}