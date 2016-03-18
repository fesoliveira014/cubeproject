#include "DrawableBox.h"

namespace tactical
{
	namespace render
	{
		DrawableBox::DrawableBox(const math::AABB & box)
		{
			m_boudingBox = box;
			GenerateMesh();
		}

		void DrawableBox::Draw(Shader & shader)
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_boudingBox.GetCenter());

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
			m_boudingBox = box;
			GenerateMesh();
		}

		void DrawableBox::GenerateMesh()
		{
			delete m_mesh.vao;
			delete m_mesh.ibo;
			m_mesh.vertices.clear();
			m_mesh.indices.clear();

			glm::vec3 v0(m_boudingBox.GetMin().x, m_boudingBox.GetMin().y, m_boudingBox.GetMax().z);
			glm::vec3 v1(m_boudingBox.GetMax().x, m_boudingBox.GetMin().y, m_boudingBox.GetMax().z);
			glm::vec3 v2(m_boudingBox.GetMin().x, m_boudingBox.GetMax().y, m_boudingBox.GetMax().z);
			glm::vec3 v3(m_boudingBox.GetMax().x, m_boudingBox.GetMax().y, m_boudingBox.GetMax().z);
			glm::vec3 v4(m_boudingBox.GetMax().x, m_boudingBox.GetMin().y, m_boudingBox.GetMin().z);
			glm::vec3 v5(m_boudingBox.GetMax().x, m_boudingBox.GetMax().y, m_boudingBox.GetMin().z);
			glm::vec3 v6(m_boudingBox.GetMin().x, m_boudingBox.GetMax().y, m_boudingBox.GetMin().z);
			glm::vec3 v7(m_boudingBox.GetMin().x, m_boudingBox.GetMin().y, m_boudingBox.GetMin().z);

			glm::vec4 color(1.0f, 1.0f, 0.0f, 0.5f);

			geometry::AddQuad(v0, v1, v2, v3, m_mesh.vertices, m_mesh.indices, color);
			geometry::AddQuad(v1, v4, v3, v5, m_mesh.vertices, m_mesh.indices, color);
			geometry::AddQuad(v4, v7, v5, v6, m_mesh.vertices, m_mesh.indices, color);
			geometry::AddQuad(v7, v0, v6, v2, m_mesh.vertices, m_mesh.indices, color);
			geometry::AddQuad(v7, v4, v0, v1, m_mesh.vertices, m_mesh.indices, color);
			geometry::AddQuad(v2, v3, v6, v5, m_mesh.vertices, m_mesh.indices, color);

			geometry::CalculateNormals<render::Vertex3f3f4f>(m_mesh.vertices, m_mesh.indices);

			std::vector<render::VertexAttribute> attributes;
			attributes.push_back(render::VertexAttribute(0, 3, GLType::FLOAT));
			attributes.push_back(render::VertexAttribute(1, 3, GLType::FLOAT));

			m_mesh.vao = new render::VertexArray();
			m_mesh.ibo = new render::IndexBuffer(m_mesh.indices.data(), (GLsizei)m_mesh.indices.size());
			m_mesh.vao->AddBuffer(new render::Buffer(m_mesh.vertices.data(), (GLsizei)m_mesh.vertices.size() * sizeof(render::Vertex3f3f), attributes));
		}
	}
}
