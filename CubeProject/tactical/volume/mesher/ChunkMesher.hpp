#ifndef _CHUNK_MESHER_H_
#define _CHUNK_MESHER_H_

#include "../../Common.h"

#include "../Chunk.h"

#include "../../render/structures/Mesh.h"
#include "../../render/geometry/Geometry.h"

namespace tactical
{
	namespace volume
	{
		namespace mesher
		{
			enum MesherType
			{
				GREEDY = 0,
				MARCHING_CUBES,
				NAIVE_WITH_CULLING
			};

			

			inline static void Greedy(Chunk& chunk)
			{
				delete chunk.GetMesh()->vao;
				delete chunk.GetMesh()->ibo;
				chunk.GetMesh()->vertices.clear();
				chunk.GetMesh()->indices.clear();
				int* mask = new int[chunk.GetSize() * chunk.GetSize()];

				int axis = 0;

				for (axis = 0; axis < 3; ++axis) {
					int u = (axis + 1) % 3;
					int v = (axis + 2) % 3;

					int x[3] = { 0, 0, 0 };
					int q[3] = { 0, 0, 0 };

					q[axis] = 1;

					for (x[axis] = -1; x[axis] < chunk.GetSize();) {
						int n = 0;
						for (x[v] = 0; x[v] < chunk.GetSize(); ++x[v]) {
							for (x[u] = 0; x[u] < chunk.GetSize(); ++x[u], ++n) {
								int a = (0 <= x[axis] ? chunk.GetVoxel(x[0], x[1], x[2]) : 0);
								int b = (x[axis] <  chunk.GetSize() - 1 ? chunk.GetVoxel(x[0] + q[0], x[1] + q[1], x[2] + q[2]) : 0);
								//int c = (x[axis] == chunk.GetSize() - 1 ? chunk.Getne)

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
						for (int j = 0; j < chunk.GetSize(); ++j) {
							for (int i = 0; i < chunk.GetSize();) {
								int c = mask[n];
								int width, height;
								if (!!c) {
									for (width = 1; c == mask[n + width] && i + width < chunk.GetSize(); ++width) {}
									bool done = false;
									for (height = 1; height + j < chunk.GetSize(); ++height) {
										for (int k = 0; k < width; ++k) {
											if (c != mask[n + k + height * chunk.GetSize()]) {
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
										glm::vec3(x[0], x[1], x[2]),
										glm::vec3(x[0] + du[0], x[1] + du[1], x[2] + du[2]),
										glm::vec3(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2]),
										glm::vec3(x[0] + dv[0], x[1] + dv[1], x[2] + dv[2]),
										chunk.GetMesh()->vertices, chunk.GetMesh()->indices);

									for (int l = 0; l < height; ++l) {
										for (int k = 0; k < width; ++k) {
											mask[n + k + l*chunk.GetSize()] = 0;
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

				render::geometry::CalculateNormals<render::Vertex3f3f>(chunk.GetMesh()->vertices, chunk.GetMesh()->indices);

				std::vector<render::VertexAttribute> attributes;
				attributes.push_back(render::VertexAttribute(0, 3, GLType::FLOAT));
				attributes.push_back(render::VertexAttribute(1, 3, GLType::FLOAT));

				chunk.GetMesh()->vao = new render::VertexArray();
				chunk.GetMesh()->ibo = new render::IndexBuffer(chunk.GetMesh()->indices.data(),
					(GLsizei)chunk.GetMesh()->indices.size());
				chunk.GetMesh()->vao->AddBuffer(new render::Buffer(chunk.GetMesh()->vertices.data(),
					(GLsizei)chunk.GetMesh()->vertices.size() * sizeof(render::Vertex3f3f), attributes));

				chunk.UpdatedGeometry();
				delete mask;
			}

			inline void Greedy2(Chunk& chunk)
			{
				delete chunk.GetMesh()->vao;
				delete chunk.GetMesh()->ibo;
				chunk.GetMesh()->vertices.clear();
				chunk.GetMesh()->indices.clear();
				int* mask = new int[chunk.GetSize() * chunk.GetSize()];

				glm::ivec3 temp(0);

				int i, j, k, l, h, w, u, v, n, r, s, t;

				render::geometry::Face face;
				int x[3] = { 0, 0, 0 };
				int q[3] = { 0, 0, 0 };
				int du[3] = { 0, 0, 0 };
				int dv[3] = { 0, 0, 0 };

				// first pass is for front face and second pass is for backface
				for (bool backface = true, b = false; b != backface; backface = backface && b, b = !b) {
					// loop over 3 dimensions
					for (int d = 0; d < 3; ++d) {
						u = (d + 1) % 3;
						v = (d + 2) % 3;

						x[0] = 0; x[1] = 0; x[2] = 0;
						q[0] = 0; q[1] = 0; q[2] = 0;
						q[d] = 1;

						// keep track of which face are we computing
						if (d == 0) {
							face.face = backface ? render::geometry::Face::RIGHT : render::geometry::Face::LEFT;
						}
						else if (d == 1) {
							face.face = backface ? render::geometry::Face::TOP : render::geometry::Face::BOTTOM;
						}
						else if (d == 2) {
							face.face = backface ? render::geometry::Face::FRONT: render::geometry::Face::BACK;
						}

						for (x[d] = 0; x[d] < chunk.GetSize(); x[d]++) {
							n = 0;
							for (x[v] = 0; x[v] < chunk.GetSize(); x[v]++) {
								for (x[u] = 0; x[u] < chunk.GetSize(); x[u]++) {
									temp = glm::ivec3(x[0], x[1], x[2]);
									mask[n++] = chunk.IsFaceVisible(temp, face) ? chunk.GetVoxel(temp) : 0;
								}
							}

							n = 0;
							for (j = 0; j < chunk.GetSize(); ++j) {
								for (i = 0; i < chunk.GetSize();) {
									if (mask[n] != 0) {
										for (w = 1; w + i < chunk.GetSize() && mask[n + w] != 0 && mask[n+w] == mask[n]; ++w) {}
										bool done = false;

										for (h = 1; j + h < chunk.GetSize(); ++h) {
											for (k = 0; k < w; ++k) {
												if (mask[n + k + h*chunk.GetSize()] == 0 || mask[n + k + h*chunk.GetSize()] != mask[n]) {
													done = true;
													break;
												}
											}
											if (done) break;
										}

										x[u] = i;
										x[v] = j;
										du[0] = 0; du[1] = 0; du[2] = 0; du[u] = w;
										dv[0] = 0; dv[1] = 0; dv[2] = 0; dv[v] = h;
										if (!backface) {
											r = x[0];
											s = x[1];
											t = x[2];
										}
										else {
											r = x[0] + q[0];
											s = x[1] + q[1];
											t = x[2] + q[2];
										}

										glm::vec3 v1(r, s, t);
										glm::vec3 v2(r + du[0], s + du[1], t + du[2]);
										glm::vec3 v3(r + dv[0], s + dv[1], t + dv[2]);
										glm::vec3 v4(r + du[0] + dv[0], s + du[1] + dv[1], t + du[2] + dv[2]);

										// Each face has a specific order of vertices, otherwise the textures rotate incorrectly
										// width/height are flipped when dealing with left/right/bottom face due to how
										// rotation of dimensions work, and what order the greedy mesher merges them.
										switch (face.face) {
										case render::geometry::Face::TOP:
											render::geometry::AddQuad<render::Vertex3f3f>(v2, v4, v1, v3, chunk.GetMesh()->vertices, chunk.GetMesh()->indices);
											// TODO: texture mapping
											break;

										case render::geometry::Face::BOTTOM:
											render::geometry::AddQuad<render::Vertex3f3f>(v4, v2, v3, v1, chunk.GetMesh()->vertices, chunk.GetMesh()->indices);
											// TODO: texture mapping
											break;

										case render::geometry::Face::LEFT:
											render::geometry::AddQuad<render::Vertex3f3f>(v1, v3, v2, v4, chunk.GetMesh()->vertices, chunk.GetMesh()->indices);
											// TODO: texture mapping
											break;

										case render::geometry::Face::RIGHT:
											render::geometry::AddQuad<render::Vertex3f3f>(v3, v1, v4, v2, chunk.GetMesh()->vertices, chunk.GetMesh()->indices);
											// TODO: texture mapping
											break;

										case render::geometry::Face::FRONT:
											render::geometry::AddQuad<render::Vertex3f3f>(v1, v2, v3, v4, chunk.GetMesh()->vertices, chunk.GetMesh()->indices);
											// TODO: texture mapping
											break;

										case render::geometry::Face::BACK:
											render::geometry::AddQuad<render::Vertex3f3f>(v2, v1, v4, v3, chunk.GetMesh()->vertices, chunk.GetMesh()->indices);
											// TODO: texture mapping
											break;
										}

										//render::geometry::AddQuad<render::Vertex3f3f>(v1, v2, v3, v4, chunk.GetMesh()->vertices, chunk.GetMesh()->indices);

										for (l = 0; l < h; ++l) {
											for (k = 0; k < w; ++k) {
												mask[n + k + l * chunk.GetSize()] = 0;
											}
										}

										i += w;
										n += w;
									}
									else {
										i++;
										n++;
									}
								}
							}
						}
					}
				}

				render::geometry::CalculateNormals<render::Vertex3f3f>(chunk.GetMesh()->vertices, chunk.GetMesh()->indices);

				std::vector<render::VertexAttribute> attributes;
				attributes.push_back(render::VertexAttribute(0, 3, GLType::FLOAT));
				attributes.push_back(render::VertexAttribute(1, 3, GLType::FLOAT));

				chunk.GetMesh()->vao = new render::VertexArray();
				chunk.GetMesh()->ibo = new render::IndexBuffer(chunk.GetMesh()->indices.data(),
					(GLsizei)chunk.GetMesh()->indices.size());
				chunk.GetMesh()->vao->AddBuffer(new render::Buffer(chunk.GetMesh()->vertices.data(),
					(GLsizei)chunk.GetMesh()->vertices.size() * sizeof(render::Vertex3f3f), attributes));

				chunk.UpdatedGeometry();
				delete mask;
			}

			inline static void MarchingCubes(Chunk& chunk)
			{
				// TODO
			}

			inline static void NaiveWithCulling(Chunk& chunk)
			{
				// TODO
			}

			inline static void GenerateChunkMesh(Chunk& chunk, MesherType type)
			{
				switch (type)
				{
				default:
				case GREEDY:
					Greedy(chunk);
					break;

				case MARCHING_CUBES:
					MarchingCubes(chunk);
					break;

				case NAIVE_WITH_CULLING:
					NaiveWithCulling(chunk);
					break;
				}
			}
		}
	}
}
#endif