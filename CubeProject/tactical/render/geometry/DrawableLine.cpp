#include "DrawableLine.h"

namespace tactical
{
	namespace render
	{
		DrawableLine::DrawableLine(const glm::vec3 & start, const glm::vec3 & end, const glm::vec4& color) :
			m_start(start),
			m_end(end),
			m_color(color)
		{
			m_mesh.vao = nullptr;
			m_mesh.ibo = nullptr;
			m_mesh.vertices.clear();
			m_mesh.indices.clear();

			GenerateMesh();
		}

		void DrawableLine::GenerateMesh()
		{
			if (m_mesh.vao != nullptr) delete m_mesh.vao;
			if (m_mesh.ibo != nullptr) delete m_mesh.ibo;
			m_mesh.vertices.clear();
			m_mesh.indices.clear();

			geometry::AddLine(m_start, m_end, m_mesh.vertices, m_mesh.indices);
			geometry::SetColor(m_color, m_mesh.vertices, m_mesh.indices);

			std::vector<render::VertexAttribute> attributes;
			attributes.push_back(render::VertexAttribute(0, 3, GLType::FLOAT));
			attributes.push_back(render::VertexAttribute(3, 4, GLType::FLOAT));

			m_mesh.vao = new render::VertexArray();
			m_mesh.ibo = new render::IndexBuffer(m_mesh.indices.data(), (GLsizei)m_mesh.indices.size());
			m_mesh.vao->AddBuffer(new render::Buffer(m_mesh.vertices.data(), (GLsizei)m_mesh.vertices.size() * sizeof(render::Vertex3f4f), attributes));
		}

		void DrawableLine::Draw(Shader& shader)
		{
			glm::mat4 model(1.0f);

			m_mesh.vao->Bind();
			m_mesh.ibo->Bind();

			shader.SetUniformMat4fv("model", model);
			shader.SetUniformBool("useColor", true);
			glDrawElements(GL_LINES, (GLsizei)m_mesh.ibo->GetCount(), GL_UNSIGNED_INT, 0);
			
			shader.SetUniformBool("useColor", false);

			m_mesh.ibo->Unbind();
			m_mesh.vao->Unbind();
		}
	}
}
