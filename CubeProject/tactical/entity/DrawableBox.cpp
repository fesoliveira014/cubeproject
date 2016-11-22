#include "DrawableBox.h"

namespace tactical
{
	namespace entity
	{
		DrawableBox::DrawableBox(const math::AABB & box)
		{
			m_boundingBox = box;

			m_mesh.vao = nullptr;
			m_mesh.ibo = nullptr;
			m_mesh.vertices.clear();
			m_mesh.indices.clear();

			m_position = glm::vec3(0.0f);

			GenerateMesh();

			m_model = glm::mat4(1.0f);
		}

		void DrawableBox::CalculateTransform()
		{
			m_model = glm::translate(glm::mat4(1.0f), m_position - glm::vec3(0.005f));
			m_model = glm::scale(m_model, glm::vec3(1.01));
		}

		void DrawableBox::Draw(render::Shader & shader)
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_position-glm::vec3(0.005f));
			model = glm::scale(model, glm::vec3(1.01));

			m_mesh.vao->Bind();
			m_mesh.ibo->Bind();
			if (!shader.IsEnabled())
				shader.Enable();

			shader.SetUniformMat4fv("model", model);
			glDrawElements(GL_TRIANGLES, (GLsizei)m_mesh.ibo->GetCount(), GL_UNSIGNED_INT, 0);

			m_mesh.ibo->Unbind();
			m_mesh.vao->Unbind();
		}

		void DrawableBox::SetAABB(const math::AABB & box)
		{
			m_boundingBox = box;
			GenerateMesh();
		}

		void DrawableBox::GenerateMesh()
		{
			if (m_mesh.vao != nullptr) delete m_mesh.vao;
			if (m_mesh.ibo != nullptr) delete m_mesh.ibo;
			m_mesh.vertices.clear();
			m_mesh.indices.clear();

			glm::vec3 mmm(0,0,0); // ---
			glm::vec3 pmm(1,0,0); // +--
			glm::vec3 pmp(1,0,1); // +-+
			glm::vec3 mmp(0,0,1); // --+
			glm::vec3 mpp(0,1,1); // -++
			glm::vec3 mpm(0,1,0); // -+-
			glm::vec3 ppm(1,1,0); // ++-
			glm::vec3 ppp(1,1,1); // +++

			render::geometry::AddQuad(mmm, mmp, mpp, mpm, m_mesh.vertices, m_mesh.indices); // left
			render::geometry::AddQuad(pmm, ppm, ppp, pmp, m_mesh.vertices, m_mesh.indices); // right
			render::geometry::AddQuad(mmm, pmm, pmp, mmp, m_mesh.vertices, m_mesh.indices); // bottom
			render::geometry::AddQuad(mpm, mpp, ppp, ppm, m_mesh.vertices, m_mesh.indices); // top
			render::geometry::AddQuad(mmm, mpm, ppm, pmm, m_mesh.vertices, m_mesh.indices); // back
			render::geometry::AddQuad(mmp, pmp, ppp, mpp, m_mesh.vertices, m_mesh.indices); // front

			render::geometry::CalculateNormals<render::Vertex3f3f>(m_mesh.vertices, m_mesh.indices);

			std::vector<render::VertexAttribute> attributes;
			attributes.push_back(render::VertexAttribute(0, 3, GLType::FLOAT));
			attributes.push_back(render::VertexAttribute(1, 3, GLType::FLOAT));

			m_mesh.vao = new render::VertexArray();
			m_mesh.ibo = new render::IndexBuffer(m_mesh.indices.data(), (GLsizei)m_mesh.indices.size());
			m_mesh.vao->AddBuffer(new render::Buffer(m_mesh.vertices.data(), (GLsizei)m_mesh.vertices.size() * sizeof(render::Vertex3f3f), attributes));
		}
	}
}
