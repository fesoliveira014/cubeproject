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

		math::RayCastResult Chunk::PickVoxel(math::Ray& ray)
		{
			math::RayCastResult result;

			auto hitTester = [](byte blockId) -> bool {return blockId != 0; };
			auto checkHit = [this,&hitTester](math::RayCastResult& result, glm::vec3 tmax, glm::vec3 pos, glm::vec3 oldPos) -> bool {
				byte outOfBounds = 0;
				byte block = GetVoxel(pos);

				if ((block != 0 && hitTester(block))) {
					result.oldPos = oldPos;
					result.pos = pos;

					float min = tmax.x;
					min = tmax.y < min ? tmax.y : min;
					min = tmax.z < min ? tmax.z : min;

					float epsilon = 0.01;
					float length = min > 0 ? glm::max(min - epsilon, 0.0f) : min;
					length = min < 0 ? glm::min(min - epsilon, 0.0f) : min;

					result.length = length;
					result.hit = true;
					return true;
				}

				return false;
			};
			
			glm::vec3 tmax;
			glm::vec3 tdelta;

			glm::vec3 step(0.0f);
			glm::vec3 cb(0.0f);

			step.x = ray.GetDirection().x > 0 ? 1 : -1;
			step.y = ray.GetDirection().y > 0 ? 1 : -1;
			step.z = ray.GetDirection().z > 0 ? 1 : -1;

			cb.x = ray.GetDirection().x > 0 ? ray.GetOrigin().x + 1 : ray.GetOrigin().x;
			cb.y = ray.GetDirection().y > 0 ? ray.GetOrigin().y + 1 : ray.GetOrigin().x;
			cb.z = ray.GetDirection().z > 0 ? ray.GetOrigin().z + 1 : ray.GetOrigin().x;

			if (ray.GetDirection().x != 0) {
				float rxr = 1.0 / ray.GetDirection().x;
				tmax.x = (cb.x - ray.GetDirection().x) * rxr;
				tdelta.x = step.x * rxr;
			}
			else
				tmax.x = 1000000;

			if (ray.GetDirection().y != 0) {
				float ryr = 1.0 / ray.GetDirection().y;
				tmax.y = (cb.y - ray.GetDirection().y) * ryr;
				tdelta.y = step.y * ryr;
			}
			else
				tmax.y = 1000000;

			if (ray.GetDirection().z != 0) {
				float rzr = 1.0 / ray.GetDirection().z;
				tmax.z = (cb.z - ray.GetDirection().z) * rzr;
				tdelta.z = step.z * rzr;
			}
			else
				tmax.z = 1000000;

			glm::vec3 origin = ray.GetOrigin();
			glm::vec3 old = origin;

			int iterations = 32;

			for (int i = 0; i < iterations; ++i) {
				if (tmax.x < tmax.y) {
					if (tmax.x < tmax.z) {
						origin.x += step.x;
						if (checkHit(result, tmax, origin, old))
							return std::move(result);
						tmax.x += tdelta.x;
					}
					else {
						origin.z += step.z;
						if (checkHit(result, tmax, origin, old))
							return std::move(result);
						tmax.z += tdelta.z;
					}
				}
				else {
					if (tmax.y < tmax.z) {
						origin.y += step.y;
						if (checkHit(result, tmax, origin, old))
							return std::move(result);
						tmax.y += tdelta.y;
					}
					else {
						origin.x += step.x;
						if (checkHit(result, tmax, origin, old))
							return std::move(result);
						tmax.x += tdelta.x;
					}
				}
				old = origin;
			}
			return std::move(result);
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

		bool Chunk::IsFaceVisible(const glm::vec3 & pos, render::geometry::Face face)
		{
			byte neighbor = 0;
			if (face.face == render::geometry::Face::TOP) {
				if (pos.y == m_size) {
					neighbor = m_neighbors[0]->GetVoxel(glm::vec3(pos.x, 0, pos.z));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(0, 1, 0));
				}
			}

			else if (face.face == render::geometry::Face::BOTTOM) {
				if (pos.y == 0) {
					neighbor = m_neighbors[0]->GetVoxel(glm::vec3(pos.x, m_size, pos.z));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(0, -1, 0));
				}
			}

			else if (face.face == render::geometry::Face::RIGHT) {
				if (pos.x == m_size) {
					neighbor = m_neighbors[0]->GetVoxel(glm::vec3(0, pos.y, pos.z));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(1, 0, 0));
				}
			}

			else if (face.face == render::geometry::Face::LEFT) {
				if (pos.x == 0) {
					neighbor = m_neighbors[0]->GetVoxel(glm::vec3(m_size, pos.y, pos.z));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(-1, 0, 0));
				}
			}

			else if (face.face == render::geometry::Face::FRONT) {
				if (pos.z == m_size) {
					neighbor = m_neighbors[0]->GetVoxel(glm::vec3(pos.x, pos.y, 0));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(0, 0, 1));
				}
			}

			else if (face.face == render::geometry::Face::BACK) {
				if (pos.z == 0) {
					neighbor = m_neighbors[0]->GetVoxel(glm::vec3(pos.x, pos.y, m_size));
				}
				else {
					neighbor = m_voxels.Get(pos + glm::vec3(0, 0, -1));
				}
			}

			return (neighbor ? true : false);
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

		void Chunk::Update()
		{
			UpdateVisibility();
			if (m_isVisible && (m_mesh.vao == nullptr || m_voxels.IsModified()))
				GenerateGeometry();
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