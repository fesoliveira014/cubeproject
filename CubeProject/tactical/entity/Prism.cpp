#include "Prism.h"

namespace tactical
{
	namespace entity
	{
		Prism::Prism(const glm::vec3& pos)
		{
			m_position = pos;
			m_model = glm::mat4(1.0f);
			m_rotation = glm::mat4(1.0f);
			m_scale = glm::vec3(1.0f);
			m_angle = 0;

			m_mesh.vao = nullptr;
			m_mesh.ibo = nullptr;
			m_mesh.vertices.clear();
			m_mesh.indices.clear();

			GenerateMesh();
			m_boundingBox.CalculateAABB<render::Vertex3f3f4f>(m_mesh.vertices);
		}

		Prism::~Prism()
		{
			if (m_mesh.vao != nullptr) delete m_mesh.vao;
			if (m_mesh.ibo != nullptr) delete m_mesh.ibo;
			m_mesh.vertices.clear();
			m_mesh.indices.clear();
		}

		void Prism::CalculateTransform()
		{
			glm::vec3 center = m_boundingBox.GetCenter() * m_scale;
			glm::vec3 dim = m_boundingBox.GetSize() * m_scale;

			m_model = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0.5f - dim.x/2, m_boundingBox.GetMax().y / 2.0f, 0.5f - dim.z/2));

			m_model = glm::translate(m_model, center);
			m_model = glm::rotate(m_model, m_angle, m_axis);
			m_model = glm::translate(m_model, -center);

			m_model = glm::scale(m_model, m_scale);
		}

		void Prism::Draw(render::Shader& shader)
		{
			CalculateTransform();

			m_mesh.vao->Bind();
			m_mesh.ibo->Bind();

			shader.SetUniformMat4fv("model", m_model);
			glDrawElements(GL_TRIANGLES, (GLsizei)m_mesh.ibo->GetCount(), GL_UNSIGNED_INT, 0);

			m_mesh.ibo->Unbind();
			m_mesh.vao->Unbind();
		}

		void Prism::GenerateMesh()
		{
			if (m_mesh.vao != nullptr) delete m_mesh.vao;
			if (m_mesh.ibo != nullptr) delete m_mesh.ibo;
			m_mesh.vertices.clear();
			m_mesh.indices.clear();

			float sqrt2 = glm::sqrt(2) / 2;

			glm::vec3 mmm(0, 0, 0);
			glm::vec3 pmm(1, 0, 0);
			glm::vec3 pmp(1, 0, 1);
			glm::vec3 mmp(0, 0, 1);
			glm::vec3 top(0.5f, 2.0f, 0.5f);
			glm::vec3 bottom(0.5f, -2.0f, 0.5f);

			render::geometry::AddTriangle<render::Vertex3f3f4f>(mmm, top, pmm, m_mesh.vertices, m_mesh.indices);
			render::geometry::AddTriangle<render::Vertex3f3f4f>(mmp, top, mmm, m_mesh.vertices, m_mesh.indices);
			render::geometry::AddTriangle<render::Vertex3f3f4f>(pmp, top, mmp, m_mesh.vertices, m_mesh.indices);
			render::geometry::AddTriangle<render::Vertex3f3f4f>(pmm, top, pmp, m_mesh.vertices, m_mesh.indices);
			render::geometry::AddTriangle<render::Vertex3f3f4f>(pmm, bottom, mmm, m_mesh.vertices, m_mesh.indices);
			render::geometry::AddTriangle<render::Vertex3f3f4f>(mmm, bottom, mmp, m_mesh.vertices, m_mesh.indices);
			render::geometry::AddTriangle<render::Vertex3f3f4f>(mmp, bottom, pmp, m_mesh.vertices, m_mesh.indices);
			render::geometry::AddTriangle<render::Vertex3f3f4f>(pmp, bottom, pmm, m_mesh.vertices, m_mesh.indices);

			render::geometry::CalculateNormals<render::Vertex3f3f4f>(m_mesh.vertices, m_mesh.indices);
			render::geometry::SetColor<render::Vertex3f3f4f>(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), m_mesh.vertices, m_mesh.indices);

			std::vector<render::VertexAttribute> attributes;
			attributes.push_back(render::VertexAttribute(render::AttributePosition::POSITION, 3, GLType::FLOAT));
			attributes.push_back(render::VertexAttribute(render::AttributePosition::NORMAL, 3, GLType::FLOAT));
			attributes.push_back(render::VertexAttribute(render::AttributePosition::COLOR, 4, GLType::FLOAT));

			m_mesh.vao = new render::VertexArray();
			m_mesh.ibo = new render::IndexBuffer(m_mesh.indices.data(), (GLsizei)m_mesh.indices.size());
			m_mesh.vao->AddBuffer(new render::Buffer(m_mesh.vertices.data(), (GLsizei)m_mesh.vertices.size() * sizeof(render::Vertex3f3f4f), attributes));
		}
	}
}