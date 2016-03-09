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
			m_isActive = false;
			m_isVisible = false;

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
				if (m_mesh.vao == nullptr || m_voxels.IsModified())
					GenerateGeometry();

				glm::mat4 model = glm::translate(glm::mat4(1.0f), m_position);

				m_mesh.vao->Bind();
				m_mesh.ibo->Bind();
				if (shader.IsEnabled())
					shader.SetUniformMat4fv("model", model);

				glDrawElements(GL_TRIANGLES, (GLsizei)m_mesh.ibo->GetCount(), GL_UNSIGNED_INT, 0);

				m_mesh.ibo->Unbind();
				m_mesh.vao->Unbind();
			}
		}

		void Chunk::GenerateGeometry()
		{
			delete m_mesh.vao;
			delete m_mesh.ibo;
			m_mesh.vertices.clear();
			m_mesh.indices.clear();
			int* mask = new int[m_size * m_size];

			int axis = 0;

			for (axis = 0; axis < 3; ++axis) {
				int u = (axis + 1) % 3;
				int v = (axis + 2) % 3;

				int x[3] = { 0, 0, 0 };
				int q[3] = { 0, 0, 0 };

				q[axis] = 1;

				for (x[axis] = -1; x[axis] < m_size;) {
					int n = 0;
					for (x[v] = 0; x[v] < m_size; ++x[v]) {
						for (x[u] = 0; x[u] < m_size; ++x[u], ++n) {
							int a = (0       <= x[axis]    ? m_voxels.Get(x[0],        x[1],        x[2])        : 0);
							int b = (x[axis] <  m_size - 1 ? m_voxels.Get(x[0] + q[0], x[1] + q[1], x[2] + q[2]) : 0);

							// These determine the normal sign
							// and if there should be placed a face
							if ((!!a) == (!!b))
								mask[n] = 0;
							else if (!!a)
								mask[n] = a;
							else
								mask[n] = -b;
						}
					}

					++x[axis];
					n = 0;
					for (int j = 0; j < m_size; ++j) {
						for (int i = 0; i < m_size;) {
							int c = mask[n];
							int width, height;
							if (!!c) {
								for (width = 1; c == mask[n + width] && i + width < m_size; ++width) {}
								bool done = false;
								for (height = 1; height + j < m_size; ++height) {
									for (int k = 0; k < width; ++k) {
										if (c != mask[n + k + height * m_size]) {
											done = true;
											break;
										}
									}

									if (done)
										break;
								}

								x[u] = i; x[v] = j;
								int du[] = { 0, 0, 0 };
								int dv[] = { 0, 0, 0 };

								if (c > 0) {
									dv[v] = height;
									du[u] = width;
								}
								else {
									c = -c;
									du[v] = height;
									dv[u] = width;
								}

								render::geometry::AddQuad<render::Vertex3f3f>(
									glm::vec3(x[0],                 x[1],                 x[2]),
									glm::vec3(x[0] + du[0],         x[1] + du[1],         x[2] + du[2]),
									glm::vec3(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2]),
									glm::vec3(x[0]         + dv[0], x[1]         + dv[1], x[2]         + dv[2]),
									m_mesh.vertices, m_mesh.indices);

								for (int l = 0; l < height; ++l) {
									for (int k = 0; k < width; ++k) {
										mask[n + k + l*m_size] = 0;
									}
								}

								i += width;
								n += width;
							}
							else {
								++i;
								++n;
							}
						}
					}
				}
			}

			render::geometry::CalculateNormals<render::Vertex3f3f>(m_mesh.vertices, m_mesh.indices);

			std::vector<render::VertexAttribute> attributes;
			attributes.push_back(render::VertexAttribute(0, 3, GLType::FLOAT));
			attributes.push_back(render::VertexAttribute(1, 3, GLType::FLOAT));

			m_mesh.vao = new render::VertexArray();
			m_mesh.ibo = new render::IndexBuffer(m_mesh.indices.data(), (GLsizei)m_mesh.indices.size());
			m_mesh.vao->AddBuffer(new render::Buffer(m_mesh.vertices.data(), (GLsizei)m_mesh.vertices.size() * sizeof(render::Vertex3f3f), attributes));

			m_voxels.Updated();
			delete mask;
		}

		void Chunk::PickVoxel(math::Ray& ray)
		{
			if (ray.Intersects(m_boudingBox)) {

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
			m_voxels.Set(position, type);
		}

		byte Chunk::GetVoxel(const glm::vec3& position)
		{
			return m_voxels.Get(position);
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

		void Chunk::Load()
		{
			GenerateGeometry();
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
		
		void Chunk::UpdateVisibility()
		{
			for (int i = 0; i < 6; ++i) {
				if (m_neighbors[i] == nullptr)
					m_isVisible = true;
			}
			m_isVisible = false;
		}
	}
}